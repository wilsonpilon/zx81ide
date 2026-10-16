#include "ui_tui/tui_app.h"
#include "core/version.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif

namespace zx81ide {
namespace ui_tui {

namespace {

// Paleta Estrita Sinclair ZX-81: Preto, Branco e Cinza
namespace Colors {
    const std::string RESET         = "\x1b[0m";
    const std::string HIDE_CURSOR   = "\x1b[?25l";
    const std::string SHOW_CURSOR   = "\x1b[?25h";
    const std::string HOME          = "\x1b[H";

    // Textos (Foreground)
    const std::string FG_BLACK      = "\x1b[30m";
    const std::string FG_WHITE      = "\x1b[97m";
    const std::string FG_GRAY_LIGHT = "\x1b[37m";
    const std::string FG_GRAY_DARK  = "\x1b[90m";

    // Fundos (Background)
    const std::string BG_BLACK      = "\x1b[40m";
    const std::string BG_WHITE      = "\x1b[107m";
    const std::string BG_GRAY_LIGHT = "\x1b[47m";
    const std::string BG_GRAY_DARK  = "\x1b[100m";
}

void setup_console() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout << Colors::HIDE_CURSOR;
    std::cout << "\x1b[2J" << Colors::HOME;
}

void restore_console() {
    std::cout << Colors::RESET << Colors::SHOW_CURSOR;
    std::cout << "\x1b[2J" << Colors::HOME;
}

void get_terminal_size(int& width, int& height) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        width = 80;
        height = 25;
    }
#else
    width = 80;
    height = 25;
#endif
    if (width < 80) width = 80;
    if (height < 24) height = 24;
}

// Representa uma célula da tela virtual
struct Cell {
    std::string ch = " ";
    std::string fg = Colors::FG_WHITE;
    std::string bg = Colors::BG_BLACK;
};

class ScreenBuffer {
public:
    int width = 80;
    int height = 25;
    std::vector<std::vector<Cell>> grid;

    ScreenBuffer(int w, int h) : width(w), height(h) {
        grid.resize(height, std::vector<Cell>(width));
    }

    void put_cell(int x, int y, const std::string& ch, const std::string& fg, const std::string& bg) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x].ch = ch;
            grid[y][x].fg = fg;
            grid[y][x].bg = bg;
        }
    }

    void put_string(int x, int y, const std::string& text, const std::string& fg, const std::string& bg) {
        int cx = x;
        for (size_t i = 0; i < text.size(); ++i) {
            if (cx >= width) break;
            std::string ch(1, text[i]);
            put_cell(cx, y, ch, fg, bg);
            cx++;
        }
    }

    void render() {
        std::string out;
        out.reserve(width * height * 15);
        out += Colors::HOME;

        std::string current_fg = "";
        std::string current_bg = "";

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const auto& cell = grid[y][x];
                if (cell.fg != current_fg) {
                    out += cell.fg;
                    current_fg = cell.fg;
                }
                if (cell.bg != current_bg) {
                    out += cell.bg;
                    current_bg = cell.bg;
                }
                out += cell.ch;
            }
            if (y < height - 1) {
                out += "\n";
            }
        }
        std::cout << out << std::flush;
    }
};

enum class MenuId {
    ARQUIVO = 0,
    HELP = 1
};

} // anonymous namespace

int run_tui() {
    setup_console();

    bool running = true;
    int current_menu = 0; // 0: Arquivo, 1: Help
    bool menu_open = false;
    bool show_about = false;

    while (running) {
        int width = 80;
        int height = 25;
        get_terminal_size(width, height);

        ScreenBuffer screen(width, height);

        // -------------------------------------------------------------
        // 1. ÁREA DE DESKTOP (Linhas 1 até height - 2)
        // Quadriculado ASCII em Cinza e Branco estilo MS-DOS dos anos 1990
        // -------------------------------------------------------------
        for (int y = 1; y < height - 1; ++y) {
            for (int x = 0; x < width; ++x) {
                // Padrão quadriculado alternado de blocos semi-transparentes (░)
                bool alternate = ((x + y) % 2 == 0);
                std::string fg = alternate ? Colors::FG_GRAY_DARK : Colors::FG_WHITE;
                std::string bg = alternate ? Colors::BG_BLACK : Colors::BG_GRAY_DARK;
                screen.put_cell(x, y, "░", fg, bg);
            }
        }

        // -------------------------------------------------------------
        // 2. BARRA DE MENU (Linha 0)
        // Fundo preto com letras em branco
        // -------------------------------------------------------------
        for (int x = 0; x < width; ++x) {
            screen.put_cell(x, 0, " ", Colors::FG_WHITE, Colors::BG_BLACK);
        }

        // Itens de Menu
        std::string menu_arquivo = " Arquivo ";
        std::string menu_help    = " Help ";

        int pos_arquivo = 1;
        int pos_help = 12;

        bool arquivo_active = (current_menu == 0);
        bool help_active    = (current_menu == 1);

        screen.put_string(pos_arquivo, 0, menu_arquivo,
            (arquivo_active ? Colors::FG_BLACK : Colors::FG_WHITE),
            (arquivo_active ? Colors::BG_WHITE : Colors::BG_BLACK));

        screen.put_string(pos_help, 0, menu_help,
            (help_active ? Colors::FG_BLACK : Colors::FG_WHITE),
            (help_active ? Colors::BG_WHITE : Colors::BG_BLACK));

        // -------------------------------------------------------------
        // 3. BARRA DE STATUS (Última Linha: height - 1)
        // Fundo cinza e letras em preto com a versão bem no final (| vX.Y.Z)
        // -------------------------------------------------------------
        int status_y = height - 1;
        for (int x = 0; x < width; ++x) {
            screen.put_cell(x, status_y, " ", Colors::FG_BLACK, Colors::BG_GRAY_LIGHT);
        }

        std::string shortcuts = " F10:Menu | Setas:Navegar | Enter:Selecionar | Esc:Fechar/Sair ";
        screen.put_string(1, status_y, shortcuts, Colors::FG_BLACK, Colors::BG_GRAY_LIGHT);

        std::string version_tag = "| v" + get_version_string();
        int version_x = width - static_cast<int>(version_tag.size()) - 1;
        if (version_x > static_cast<int>(shortcuts.size())) {
            screen.put_string(version_x, status_y, version_tag, Colors::FG_BLACK, Colors::BG_GRAY_LIGHT);
        }

        // -------------------------------------------------------------
        // 4. SUBMENU DROPDOWN
        // -------------------------------------------------------------
        if (menu_open && !show_about) {
            if (current_menu == 0) {
                // Dropdown Arquivo -> [ Sair ]
                int drop_x = pos_arquivo;
                int drop_y = 1;
                int drop_w = 20;

                // Caixa com borda simples
                screen.put_string(drop_x, drop_y,     "┌──────────────────┐", Colors::FG_WHITE, Colors::BG_BLACK);
                screen.put_string(drop_x, drop_y + 1, "│                  │", Colors::FG_WHITE, Colors::BG_BLACK);
                screen.put_string(drop_x, drop_y + 2, "└──────────────────┘", Colors::FG_WHITE, Colors::BG_BLACK);

                // Item Sair selecionado
                screen.put_string(drop_x + 2, drop_y + 1, " Sair       Alt+X ", Colors::FG_BLACK, Colors::BG_WHITE);

                // Sombra MS-DOS (drop shadow)
                screen.put_cell(drop_x + drop_w, drop_y + 1, "▒", Colors::FG_GRAY_DARK, Colors::BG_BLACK);
                screen.put_cell(drop_x + drop_w, drop_y + 2, "▒", Colors::FG_GRAY_DARK, Colors::BG_BLACK);
                for (int sx = 1; sx <= drop_w; ++sx) {
                    screen.put_cell(drop_x + sx, drop_y + 3, "▒", Colors::FG_GRAY_DARK, Colors::BG_BLACK);
                }
            } else if (current_menu == 1) {
                // Dropdown Help -> [ About ]
                int drop_x = pos_help;
                int drop_y = 1;
                int drop_w = 20;

                screen.put_string(drop_x, drop_y,     "┌──────────────────┐", Colors::FG_WHITE, Colors::BG_BLACK);
                screen.put_string(drop_x, drop_y + 1, "│                  │", Colors::FG_WHITE, Colors::BG_BLACK);
                screen.put_string(drop_x, drop_y + 2, "└──────────────────┘", Colors::FG_WHITE, Colors::BG_BLACK);

                screen.put_string(drop_x + 2, drop_y + 1, " About...      F1 ", Colors::FG_BLACK, Colors::BG_WHITE);

                // Sombra MS-DOS
                screen.put_cell(drop_x + drop_w, drop_y + 1, "▒", Colors::FG_GRAY_DARK, Colors::BG_BLACK);
                screen.put_cell(drop_x + drop_w, drop_y + 2, "▒", Colors::FG_GRAY_DARK, Colors::BG_BLACK);
                for (int sx = 1; sx <= drop_w; ++sx) {
                    screen.put_cell(drop_x + sx, drop_y + 3, "▒", Colors::FG_GRAY_DARK, Colors::BG_BLACK);
                }
            }
        }

        // -------------------------------------------------------------
        // 5. CAIXA DE DIÁLOGO MODAL: HELP -> ABOUT
        // -------------------------------------------------------------
        if (show_about) {
            int dlg_w = 54;
            int dlg_h = 16;
            int dlg_x = (width - dlg_w) / 2;
            int dlg_y = (height - dlg_h) / 2;

            // Fundo da caixa
            for (int dy = 0; dy < dlg_h; ++dy) {
                for (int dx = 0; dx < dlg_w; ++dx) {
                    screen.put_cell(dlg_x + dx, dlg_y + dy, " ", Colors::FG_WHITE, Colors::BG_BLACK);
                }
            }

            // Moldura dupla estilo Turbo Vision / MS-DOS
            screen.put_string(dlg_x, dlg_y, "╔" + std::string(dlg_w - 2, '=') + "╗", Colors::FG_WHITE, Colors::BG_BLACK);
            for (int dy = 1; dy < dlg_h - 1; ++dy) {
                screen.put_cell(dlg_x, dlg_y + dy, "║", Colors::FG_WHITE, Colors::BG_BLACK);
                screen.put_cell(dlg_x + dlg_w - 1, dlg_y + dy, "║", Colors::FG_WHITE, Colors::BG_BLACK);
            }
            screen.put_string(dlg_x, dlg_y + dlg_h - 1, "╚" + std::string(dlg_w - 2, '=') + "╝", Colors::FG_WHITE, Colors::BG_BLACK);

            // Título
            std::string title = "[ Sobre o zx81ide ]";
            screen.put_string(dlg_x + (dlg_w - static_cast<int>(title.size())) / 2, dlg_y, title, Colors::FG_WHITE, Colors::BG_BLACK);

            // Conteúdo
            screen.put_string(dlg_x + 3, dlg_y + 2,  "zx81ide - IDE para Sinclair ZX-81 & Clones", Colors::FG_WHITE, Colors::BG_BLACK);
            screen.put_string(dlg_x + 3, dlg_y + 3,  "Linha Sinclair ZX-81, TK82/83/85, CP200, TS1000", Colors::FG_GRAY_DARK, Colors::BG_BLACK);
            screen.put_string(dlg_x + 3, dlg_y + 5,  "Versao      : v" + get_version_string(), Colors::FG_WHITE, Colors::BG_BLACK);
            screen.put_string(dlg_x + 3, dlg_y + 6,  "Hardware (X): " + VERSION_MAJOR_NAME, Colors::FG_GRAY_LIGHT, Colors::BG_BLACK);
            screen.put_string(dlg_x + 3, dlg_y + 7,  "Jogo     (Y): " + VERSION_GAME_CODENAME, Colors::FG_GRAY_LIGHT, Colors::BG_BLACK);
            screen.put_string(dlg_x + 3, dlg_y + 8,  "Build    (Z): " + VERSION_BUILD_SUBTITLE, Colors::FG_GRAY_DARK, Colors::BG_BLACK);
            screen.put_string(dlg_x + 3, dlg_y + 10, "Autor       : Wilson \"Barney\" Pilon", Colors::FG_WHITE, Colors::BG_BLACK);
            screen.put_string(dlg_x + 3, dlg_y + 11, "Inspiracao  : fwMSX (github.com/wilsonpilon/fwMSX)", Colors::FG_GRAY_DARK, Colors::BG_BLACK);

            // Botão [ OK ]
            std::string ok_btn = " [  OK  ] ";
            screen.put_string(dlg_x + (dlg_w - static_cast<int>(ok_btn.size())) / 2, dlg_y + 13, ok_btn, Colors::FG_BLACK, Colors::BG_WHITE);

            // Sombra do diálogo
            for (int dy = 1; dy <= dlg_h; ++dy) {
                screen.put_cell(dlg_x + dlg_w, dlg_y + dy, "▒", Colors::FG_GRAY_DARK, Colors::BG_BLACK);
                screen.put_cell(dlg_x + dlg_w + 1, dlg_y + dy, "▒", Colors::FG_GRAY_DARK, Colors::BG_BLACK);
            }
            for (int dx = 2; dx <= dlg_w + 1; ++dx) {
                screen.put_cell(dlg_x + dx, dlg_y + dlg_h, "▒", Colors::FG_GRAY_DARK, Colors::BG_BLACK);
            }
        }

        // Renderiza o buffer completo na tela
        screen.render();

        // -------------------------------------------------------------
        // 6. PROCESSAMENTO DE ENTRADA DO TECLADO
        // -------------------------------------------------------------
#ifdef _WIN32
        int ch = _getch();
        if (ch == 0 || ch == 224) {
            int ext = _getch();
            if (ext == 75) { // Seta Esquerda
                if (show_about) {
                    // Sem ação no diálogo simples
                } else {
                    current_menu = (current_menu == 0) ? 1 : 0;
                }
            } else if (ext == 77) { // Seta Direita
                if (show_about) {
                    // Sem ação no diálogo simples
                } else {
                    current_menu = (current_menu == 0) ? 1 : 0;
                }
            } else if (ext == 80) { // Seta Abaixo
                if (!show_about) {
                    menu_open = true;
                }
            } else if (ext == 72) { // Seta Acima
                if (!show_about && menu_open) {
                    menu_open = false;
                }
            } else if (ext == 59) { // F1
                show_about = true;
                menu_open = false;
            } else if (ext == 68) { // F10
                menu_open = !menu_open;
            }
        } else {
            if (ch == 27) { // ESC
                if (show_about) {
                    show_about = false;
                } else if (menu_open) {
                    menu_open = false;
                } else {
                    // Sai do modo TUI e retorna
                    running = false;
                }
            } else if (ch == 13 || ch == 10 || ch == ' ') { // ENTER ou ESPAÇO
                if (show_about) {
                    show_about = false;
                } else if (!menu_open) {
                    menu_open = true;
                } else {
                    if (current_menu == 0) {
                        // Arquivo -> Sair
                        running = false;
                    } else if (current_menu == 1) {
                        // Help -> About
                        show_about = true;
                        menu_open = false;
                    }
                }
            } else if (ch == 'a' || ch == 'A') {
                current_menu = 0;
                menu_open = true;
            } else if (ch == 'h' || ch == 'H') {
                current_menu = 1;
                menu_open = true;
            } else if (ch == 's' || ch == 'S') {
                if (menu_open && current_menu == 0) {
                    running = false;
                }
            } else if (ch == 'x' || ch == 'X' || ch == 'q' || ch == 'Q') {
                // Atalho rápido para sair
                running = false;
            }
        }
#else
        // Plataformas sem conio.h: aguarda enter básico
        char c;
        if (std::cin >> c) {
            if (c == 'q' || c == 'x') running = false;
        }
#endif
    }

    restore_console();
    return 0;
}

} // namespace ui_tui
} // namespace zx81ide
