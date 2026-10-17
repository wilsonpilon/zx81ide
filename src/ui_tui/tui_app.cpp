#include "ui_tui/tui_app.h"
#include "core/version.h"
#include "core/config_manager.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>
#include <algorithm>

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

struct TuiTheme {
    std::string name;

    // Desktop
    std::string desktop_char;
    std::string desktop_fg;
    std::string desktop_bg;

    // Barra de Menus (Linha 0)
    std::string menu_bar_fg;
    std::string menu_bar_bg;
    std::string menu_active_fg;
    std::string menu_active_bg;

    // Submenu Dropdown (Bordas, interior e seleção)
    std::string drop_border_fg;
    std::string drop_border_bg;
    std::string drop_item_fg;
    std::string drop_item_bg;
    std::string drop_sel_fg;
    std::string drop_sel_bg;
    std::string drop_shadow_fg;
    std::string drop_shadow_bg;

    // Barra de Status (Última linha)
    std::string status_fg;
    std::string status_bg;

    // Diálogo About
    std::string dlg_border_fg;
    std::string dlg_border_bg;
    std::string dlg_body_bg;
    std::string dlg_title_fg;
    std::string dlg_title_bg;
    std::string dlg_text_fg;
    std::string dlg_text_dim;
    std::string dlg_btn_fg;
    std::string dlg_btn_bg;
    std::string dlg_shadow_fg;
    std::string dlg_shadow_bg;
};

TuiTheme get_tui_theme(const std::string& theme_name) {
    std::string name = theme_name;
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    TuiTheme th;
    th.name = name;

    if (name == "invertido") {
        // =============================================================
        // TEMA INVERTIDO SINCLAIR ZX-81:
        // - Menus com fundo cinza, e letras pretas
        // - Destaques / selecao com fundo preto e letras brancas
        // - Desktop inverte as cores do normal (FG preto, BG cinza escuro)
        // - Barra de status com fundo preto e letras brancas
        // =============================================================
        th.desktop_char    = "░";
        th.desktop_fg      = Colors::FG_BLACK;
        th.desktop_bg      = Colors::BG_GRAY_DARK;

        th.menu_bar_fg     = Colors::FG_BLACK;
        th.menu_bar_bg     = Colors::BG_GRAY_LIGHT;
        th.menu_active_fg  = Colors::FG_WHITE;
        th.menu_active_bg  = Colors::BG_BLACK;

        th.drop_border_fg  = Colors::FG_BLACK;
        th.drop_border_bg  = Colors::BG_GRAY_LIGHT;
        th.drop_item_fg    = Colors::FG_BLACK;
        th.drop_item_bg    = Colors::BG_GRAY_LIGHT;
        th.drop_sel_fg     = Colors::FG_WHITE;
        th.drop_sel_bg     = Colors::BG_BLACK;
        th.drop_shadow_fg  = Colors::FG_BLACK;
        th.drop_shadow_bg  = Colors::BG_GRAY_DARK;

        th.status_fg       = Colors::FG_WHITE;
        th.status_bg       = Colors::BG_BLACK;

        th.dlg_border_fg   = Colors::FG_BLACK;
        th.dlg_border_bg   = Colors::BG_GRAY_LIGHT;
        th.dlg_body_bg     = Colors::BG_GRAY_LIGHT;
        th.dlg_title_fg    = Colors::FG_WHITE;
        th.dlg_title_bg    = Colors::BG_BLACK;
        th.dlg_text_fg     = Colors::FG_BLACK;
        th.dlg_text_dim    = Colors::FG_GRAY_DARK;
        th.dlg_btn_fg      = Colors::FG_WHITE;
        th.dlg_btn_bg      = Colors::BG_BLACK;
        th.dlg_shadow_fg   = Colors::FG_BLACK;
        th.dlg_shadow_bg   = Colors::BG_GRAY_DARK;
    } else if (name == "escuro") {
        // =============================================================
        // TEMA ESCURO SINCLAIR ZX-81:
        // - Desktop com cores preta e cinza
        // - Barra de menus com fundo preto e letras em branco
        // - Destaques / selecao em branco com letras em preto
        // - Barra de status com fundo preto e letras em branco
        // =============================================================
        th.desktop_char    = "░";
        th.desktop_fg      = Colors::FG_GRAY_DARK;
        th.desktop_bg      = Colors::BG_BLACK;

        th.menu_bar_fg     = Colors::FG_WHITE;
        th.menu_bar_bg     = Colors::BG_BLACK;
        th.menu_active_fg  = Colors::FG_BLACK;
        th.menu_active_bg  = Colors::BG_WHITE;

        th.drop_border_fg  = Colors::FG_WHITE;
        th.drop_border_bg  = Colors::BG_BLACK;
        th.drop_item_fg    = Colors::FG_WHITE;
        th.drop_item_bg    = Colors::BG_BLACK;
        th.drop_sel_fg     = Colors::FG_BLACK;
        th.drop_sel_bg     = Colors::BG_WHITE;
        th.drop_shadow_fg  = Colors::FG_GRAY_DARK;
        th.drop_shadow_bg  = Colors::BG_BLACK;

        th.status_fg       = Colors::FG_WHITE;
        th.status_bg       = Colors::BG_BLACK;

        th.dlg_border_fg   = Colors::FG_WHITE;
        th.dlg_border_bg   = Colors::BG_BLACK;
        th.dlg_body_bg     = Colors::BG_BLACK;
        th.dlg_title_fg    = Colors::FG_BLACK;
        th.dlg_title_bg    = Colors::BG_WHITE;
        th.dlg_text_fg     = Colors::FG_WHITE;
        th.dlg_text_dim    = Colors::FG_GRAY_DARK;
        th.dlg_btn_fg      = Colors::FG_BLACK;
        th.dlg_btn_bg      = Colors::BG_WHITE;
        th.dlg_shadow_fg   = Colors::FG_GRAY_DARK;
        th.dlg_shadow_bg   = Colors::BG_BLACK;
    } else if (name == "claro") {
        // =============================================================
        // TEMA CLARO SINCLAIR ZX-81:
        // - Desktop com letra e fundo cinzas
        // - Barra de status com fundo branco e letras pretas
        // - Barra de menus com fundo branco e letras pretas
        // - Destaques / selecao com fundo cinza e letras brancas
        // =============================================================
        th.desktop_char    = "░";
        th.desktop_fg      = Colors::FG_GRAY_DARK;
        th.desktop_bg      = Colors::BG_GRAY_LIGHT;

        th.menu_bar_fg     = Colors::FG_BLACK;
        th.menu_bar_bg     = Colors::BG_WHITE;
        th.menu_active_fg  = Colors::FG_WHITE;
        th.menu_active_bg  = Colors::BG_GRAY_DARK;

        th.drop_border_fg  = Colors::FG_BLACK;
        th.drop_border_bg  = Colors::BG_WHITE;
        th.drop_item_fg    = Colors::FG_BLACK;
        th.drop_item_bg    = Colors::BG_WHITE;
        th.drop_sel_fg     = Colors::FG_WHITE;
        th.drop_sel_bg     = Colors::BG_GRAY_DARK;
        th.drop_shadow_fg  = Colors::FG_GRAY_DARK;
        th.drop_shadow_bg  = Colors::BG_GRAY_LIGHT;

        th.status_fg       = Colors::FG_BLACK;
        th.status_bg       = Colors::BG_WHITE;

        th.dlg_border_fg   = Colors::FG_BLACK;
        th.dlg_border_bg   = Colors::BG_WHITE;
        th.dlg_body_bg     = Colors::BG_WHITE;
        th.dlg_title_fg    = Colors::FG_WHITE;
        th.dlg_title_bg    = Colors::BG_GRAY_DARK;
        th.dlg_text_fg     = Colors::FG_BLACK;
        th.dlg_text_dim    = Colors::FG_GRAY_DARK;
        th.dlg_btn_fg      = Colors::FG_WHITE;
        th.dlg_btn_bg      = Colors::BG_GRAY_DARK;
        th.dlg_shadow_fg   = Colors::FG_GRAY_DARK;
        th.dlg_shadow_bg   = Colors::BG_GRAY_LIGHT;
    } else {
        // Padrao "normal" (Sinclair ZX-81 Original)
        th.name            = "normal";
        th.desktop_char    = "░";
        th.desktop_fg      = Colors::FG_GRAY_DARK;
        th.desktop_bg      = Colors::BG_BLACK;

        th.menu_bar_fg     = Colors::FG_WHITE;
        th.menu_bar_bg     = Colors::BG_BLACK;
        th.menu_active_fg  = Colors::FG_BLACK;
        th.menu_active_bg  = Colors::BG_WHITE;

        th.drop_border_fg  = Colors::FG_WHITE;
        th.drop_border_bg  = Colors::BG_BLACK;
        th.drop_item_fg    = Colors::FG_WHITE;
        th.drop_item_bg    = Colors::BG_BLACK;
        th.drop_sel_fg     = Colors::FG_BLACK;
        th.drop_sel_bg     = Colors::BG_WHITE;
        th.drop_shadow_fg  = Colors::FG_GRAY_DARK;
        th.drop_shadow_bg  = Colors::BG_BLACK;

        th.status_fg       = Colors::FG_BLACK;
        th.status_bg       = Colors::BG_GRAY_LIGHT;

        th.dlg_border_fg   = Colors::FG_WHITE;
        th.dlg_border_bg   = Colors::BG_BLACK;
        th.dlg_body_bg     = Colors::BG_BLACK;
        th.dlg_title_fg    = Colors::FG_WHITE;
        th.dlg_title_bg    = Colors::BG_BLACK;
        th.dlg_text_fg     = Colors::FG_WHITE;
        th.dlg_text_dim    = Colors::FG_GRAY_DARK;
        th.dlg_btn_fg      = Colors::FG_BLACK;
        th.dlg_btn_bg      = Colors::BG_WHITE;
        th.dlg_shadow_fg   = Colors::FG_GRAY_DARK;
        th.dlg_shadow_bg   = Colors::BG_BLACK;
    }

    return th;
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
    SetConsoleCP(CP_UTF8);
#endif
    std::cout << Colors::HIDE_CURSOR;
    std::cout << "\x1b[2J" << Colors::HOME;
}

std::string repeat_utf8(const std::string& ch, int count) {
    std::string result;
    result.reserve(ch.size() * count);
    for (int i = 0; i < count; ++i) {
        result += ch;
    }
    return result;
}

std::string format_menu_item(const std::string& label, const std::string& shortcut, int inner_width) {
    std::string text = " " + label;
    int spaces = inner_width - static_cast<int>(text.size()) - static_cast<int>(shortcut.size()) - 1;
    if (spaces < 1) spaces = 1;
    text += std::string(spaces, ' ') + shortcut + " ";
    if (static_cast<int>(text.size()) > inner_width) {
        text = text.substr(0, inner_width);
    }
    return text;
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

    void put_string(int x, int y, const std::string& str, const std::string& fg, const std::string& bg) {
        int cur_x = x;
        size_t i = 0;
        size_t len = str.size();

        while (i < len) {
            if (cur_x >= width) break;

            unsigned char c = static_cast<unsigned char>(str[i]);
            size_t char_bytes = 1;

            if ((c & 0x80) == 0) {
                char_bytes = 1;
            } else if ((c & 0xE0) == 0xC0) {
                char_bytes = 2;
            } else if ((c & 0xF0) == 0xE0) {
                char_bytes = 3;
            } else if ((c & 0xF8) == 0xF0) {
                char_bytes = 4;
            }

            if (i + char_bytes > len) {
                char_bytes = 1;
            }

            std::string utf8_ch = str.substr(i, char_bytes);
            put_cell(cur_x, y, utf8_ch, fg, bg);

            cur_x++;
            i += char_bytes;
        }
    }

    void render() {
        std::string out;
        out.reserve(width * height * 16);
        out += Colors::HOME;

        std::string last_fg = "";
        std::string last_bg = "";

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const Cell& cell = grid[y][x];

                if (cell.fg != last_fg) {
                    out += cell.fg;
                    last_fg = cell.fg;
                }
                if (cell.bg != last_bg) {
                    out += cell.bg;
                    last_bg = cell.bg;
                }
                out += cell.ch;
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

    std::string current_theme_name = core::ConfigManager::instance().get_theme();
    TuiTheme theme = get_tui_theme(current_theme_name);

    while (running) {
        int width = 80;
        int height = 25;
        get_terminal_size(width, height);

        ScreenBuffer screen(width, height);

        // -------------------------------------------------------------
        // 1. ÁREA DE DESKTOP (Linhas 1 até height - 2)
        // Quadriculado ASCII com cores do tema selecionado
        // -------------------------------------------------------------
        for (int y = 1; y < height - 1; ++y) {
            for (int x = 0; x < width; ++x) {
                screen.put_cell(x, y, theme.desktop_char, theme.desktop_fg, theme.desktop_bg);
            }
        }

        // -------------------------------------------------------------
        // 2. BARRA DE MENU (Linha 0)
        // Cores configuradas pelo tema ativo
        // -------------------------------------------------------------
        for (int x = 0; x < width; ++x) {
            screen.put_cell(x, 0, " ", theme.menu_bar_fg, theme.menu_bar_bg);
        }

        // Itens de Menu
        std::string menu_arquivo = " Arquivo ";
        std::string menu_help    = " Help ";

        int pos_arquivo = 1;
        int pos_help = 12;

        bool arquivo_active = (current_menu == 0);
        bool help_active    = (current_menu == 1);

        screen.put_string(pos_arquivo, 0, menu_arquivo,
            (arquivo_active ? theme.menu_active_fg : theme.menu_bar_fg),
            (arquivo_active ? theme.menu_active_bg : theme.menu_bar_bg));

        screen.put_string(pos_help, 0, menu_help,
            (help_active ? theme.menu_active_fg : theme.menu_bar_fg),
            (help_active ? theme.menu_active_bg : theme.menu_bar_bg));

        // -------------------------------------------------------------
        // 3. BARRA DE STATUS (Última Linha: height - 1)
        // Fundo e letras conforme o tema com atalhos e versao
        // -------------------------------------------------------------
        int status_y = height - 1;
        for (int x = 0; x < width; ++x) {
            screen.put_cell(x, status_y, " ", theme.status_fg, theme.status_bg);
        }

        std::string shortcuts = " F10:Menu | T:Tema | Setas:Navegar | Enter:Selecionar | Esc:Fechar/Sair ";
        screen.put_string(1, status_y, shortcuts, theme.status_fg, theme.status_bg);

        std::string version_tag = "| Tema: " + theme.name + " | v" + get_version_string();
        int version_x = width - static_cast<int>(version_tag.size()) - 1;
        if (version_x > static_cast<int>(shortcuts.size())) {
            screen.put_string(version_x, status_y, version_tag, theme.status_fg, theme.status_bg);
        }

        // -------------------------------------------------------------
        // 4. SUBMENU DROPDOWN
        // -------------------------------------------------------------
        if (menu_open && !show_about) {
            int drop_w = 22;
            int inner_w = drop_w - 2; // 20 colunas internas

            if (current_menu == 0) {
                // Dropdown Arquivo -> [ Sair ]
                int drop_x = pos_arquivo;
                int drop_y = 1;

                // Caixa com borda simples UTF-8
                screen.put_string(drop_x, drop_y,     "┌" + repeat_utf8("─", inner_w) + "┐", theme.drop_border_fg, theme.drop_border_bg);
                screen.put_string(drop_x, drop_y + 1, "│" + std::string(inner_w, ' ')  + "│", theme.drop_border_fg, theme.drop_border_bg);
                screen.put_string(drop_x, drop_y + 2, "└" + repeat_utf8("─", inner_w) + "┘", theme.drop_border_fg, theme.drop_border_bg);

                // Item Sair selecionado
                std::string item_sair = format_menu_item("Sair", "Alt+X", inner_w);
                screen.put_string(drop_x + 1, drop_y + 1, item_sair, theme.drop_sel_fg, theme.drop_sel_bg);

                // Sombra MS-DOS (drop shadow)
                screen.put_cell(drop_x + drop_w, drop_y + 1, "▒", theme.drop_shadow_fg, theme.drop_shadow_bg);
                screen.put_cell(drop_x + drop_w, drop_y + 2, "▒", theme.drop_shadow_fg, theme.drop_shadow_bg);
                for (int sx = 1; sx <= drop_w; ++sx) {
                    screen.put_cell(drop_x + sx, drop_y + 3, "▒", theme.drop_shadow_fg, theme.drop_shadow_bg);
                }
            } else if (current_menu == 1) {
                // Dropdown Help -> [ About ]
                int drop_x = pos_help;
                int drop_y = 1;

                screen.put_string(drop_x, drop_y,     "┌" + repeat_utf8("─", inner_w) + "┐", theme.drop_border_fg, theme.drop_border_bg);
                screen.put_string(drop_x, drop_y + 1, "│" + std::string(inner_w, ' ')  + "│", theme.drop_border_fg, theme.drop_border_bg);
                screen.put_string(drop_x, drop_y + 2, "└" + repeat_utf8("─", inner_w) + "┘", theme.drop_border_fg, theme.drop_border_bg);

                std::string item_about = format_menu_item("About...", "F1", inner_w);
                screen.put_string(drop_x + 1, drop_y + 1, item_about, theme.drop_sel_fg, theme.drop_sel_bg);

                // Sombra MS-DOS
                screen.put_cell(drop_x + drop_w, drop_y + 1, "▒", theme.drop_shadow_fg, theme.drop_shadow_bg);
                screen.put_cell(drop_x + drop_w, drop_y + 2, "▒", theme.drop_shadow_fg, theme.drop_shadow_bg);
                for (int sx = 1; sx <= drop_w; ++sx) {
                    screen.put_cell(drop_x + sx, drop_y + 3, "▒", theme.drop_shadow_fg, theme.drop_shadow_bg);
                }
            }
        }

        // -------------------------------------------------------------
        // 5. CAIXA DE DIÁLOGO MODAL: HELP -> ABOUT
        // -------------------------------------------------------------
        if (show_about) {
            int dlg_w = 54;
            int dlg_h = 17;
            int dlg_x = (width - dlg_w) / 2;
            int dlg_y = (height - dlg_h) / 2;

            // Fundo da caixa
            for (int dy = 0; dy < dlg_h; ++dy) {
                for (int dx = 0; dx < dlg_w; ++dx) {
                    screen.put_cell(dlg_x + dx, dlg_y + dy, " ", theme.dlg_text_fg, theme.dlg_body_bg);
                }
            }

            // Moldura dupla estilo Turbo Vision / MS-DOS
            screen.put_string(dlg_x, dlg_y, "╔" + repeat_utf8("═", dlg_w - 2) + "╗", theme.dlg_border_fg, theme.dlg_border_bg);
            for (int dy = 1; dy < dlg_h - 1; ++dy) {
                screen.put_cell(dlg_x, dlg_y + dy, "║", theme.dlg_border_fg, theme.dlg_border_bg);
                screen.put_cell(dlg_x + dlg_w - 1, dlg_y + dy, "║", theme.dlg_border_fg, theme.dlg_border_bg);
            }
            screen.put_string(dlg_x, dlg_y + dlg_h - 1, "╚" + repeat_utf8("═", dlg_w - 2) + "╝", theme.dlg_border_fg, theme.dlg_border_bg);

            // Título
            std::string title = "[ Sobre o zx81ide ]";
            screen.put_string(dlg_x + (dlg_w - static_cast<int>(title.size())) / 2, dlg_y, title, theme.dlg_title_fg, theme.dlg_title_bg);

            // Conteúdo
            screen.put_string(dlg_x + 3, dlg_y + 2,  "zx81ide - IDE para Sinclair ZX-81 & Clones", theme.dlg_text_fg, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 3,  "Linha Sinclair ZX-81, TK82/83/85, CP200, TS1000", theme.dlg_text_dim, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 5,  "Versao      : v" + get_version_string(), theme.dlg_text_fg, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 6,  "Hardware (X): " + VERSION_MAJOR_NAME, theme.dlg_text_dim, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 7,  "Jogo     (Y): " + VERSION_GAME_CODENAME, theme.dlg_text_dim, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 8,  "Build    (Z): " + VERSION_BUILD_SUBTITLE, theme.dlg_text_dim, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 9,  "Tema Ativo  : " + theme.name + " (SQLite 3)", theme.dlg_text_fg, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 11, "Autor       : Wilson \"Barney\" Pilon", theme.dlg_text_fg, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 12, "Inspiracao  : fwMSX (github.com/wilsonpilon/fwMSX)", theme.dlg_text_dim, theme.dlg_body_bg);

            // Botão [ OK ]
            std::string ok_btn = " [  OK  ] ";
            screen.put_string(dlg_x + (dlg_w - static_cast<int>(ok_btn.size())) / 2, dlg_y + 14, ok_btn, theme.dlg_btn_fg, theme.dlg_btn_bg);

            // Sombra do diálogo
            for (int dy = 1; dy <= dlg_h; ++dy) {
                screen.put_cell(dlg_x + dlg_w, dlg_y + dy, "▒", theme.dlg_shadow_fg, theme.dlg_shadow_bg);
                screen.put_cell(dlg_x + dlg_w + 1, dlg_y + dy, "▒", theme.dlg_shadow_fg, theme.dlg_shadow_bg);
            }
            for (int dx = 2; dx <= dlg_w + 1; ++dx) {
                screen.put_cell(dlg_x + dx, dlg_y + dlg_h, "▒", theme.dlg_shadow_fg, theme.dlg_shadow_bg);
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
                if (!show_about) {
                    current_menu = (current_menu == 0) ? 1 : 0;
                }
            } else if (ext == 77) { // Seta Direita
                if (!show_about) {
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
            } else if (ext == 59) { // F1: About
                show_about = true;
                menu_open = false;
            } else if (ext == 60) { // F2: Alternar Tema
                std::string next_theme = "normal";
                if (current_theme_name == "normal") next_theme = "invertido";
                else if (current_theme_name == "invertido") next_theme = "claro";
                else if (current_theme_name == "claro") next_theme = "escuro";
                else next_theme = "normal";

                core::ConfigManager::instance().set_theme(next_theme);
                current_theme_name = next_theme;
                theme = get_tui_theme(current_theme_name);
            } else if (ext == 68) { // F10: Menu
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
            } else if (ch == 't' || ch == 'T') {
                // Alternar tema diretamente com a tecla T
                std::string next_theme = "normal";
                if (current_theme_name == "normal") next_theme = "invertido";
                else if (current_theme_name == "invertido") next_theme = "claro";
                else if (current_theme_name == "claro") next_theme = "escuro";
                else next_theme = "normal";

                core::ConfigManager::instance().set_theme(next_theme);
                current_theme_name = next_theme;
                theme = get_tui_theme(current_theme_name);
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
