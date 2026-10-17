#include "ui_tui/tui_app.h"
#include "core/version.h"
#include "core/config_manager.h"
#include "editor/text_editor.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
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

#ifdef _WIN32
DWORD g_prev_in_mode = 0;
DWORD g_prev_out_mode = 0;
CONSOLE_CURSOR_INFO g_prev_cci;
bool g_has_prev_cci = false;
bool g_console_initialized = false;
#endif

void setup_console() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            if (!g_console_initialized) {
                g_prev_out_mode = dwMode;
            }
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
        CONSOLE_CURSOR_INFO cci;
        if (GetConsoleCursorInfo(hOut, &cci)) {
            if (!g_console_initialized) {
                g_prev_cci = cci;
                g_has_prev_cci = true;
            }
            cci.dwSize = 100; // Quadrado solido 100% estilo MS-DOS
            SetConsoleCursorInfo(hOut, &cci);
        }
    }
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hIn != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hIn, &dwMode)) {
            if (!g_console_initialized) {
                g_prev_in_mode = dwMode;
                g_console_initialized = true;
            }
            dwMode |= ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_EXTENDED_FLAGS;
            dwMode &= ~ENABLE_QUICK_EDIT_MODE;
            SetConsoleMode(hIn, dwMode);
        }
    }
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    std::cout << "\x1b[1 q" << Colors::HIDE_CURSOR;
    std::cout << "\x1b[2J" << Colors::HOME << std::flush;
}

void restore_console() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        if (g_has_prev_cci) {
            SetConsoleCursorInfo(hOut, &g_prev_cci);
        }
        if (g_prev_out_mode != 0) {
            SetConsoleMode(hOut, g_prev_out_mode);
        }
    }
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hIn != INVALID_HANDLE_VALUE && g_prev_in_mode != 0) {
        SetConsoleMode(hIn, g_prev_in_mode);
    }
#endif
    std::cout << "\x1b[0 q" << Colors::RESET << Colors::SHOW_CURSOR;
    std::cout << "\x1b[2J" << Colors::HOME << std::flush;
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

struct TuiMenuItem {
    std::string label;
    std::string shortcut;
    int id;
};

struct TuiMenu {
    std::string title;
    int x_pos;
    int width;
    std::vector<TuiMenuItem> items;
};

std::vector<TuiMenu> build_tui_menus() {
    std::vector<TuiMenu> menus;

    TuiMenu m_file;
    m_file.title = "Arquivo";
    m_file.x_pos = 1;
    m_file.width = 9; // " Arquivo "
    m_file.items = {
        {"Editor Livre", "F3", 101},
        {"Sair da TUI", "Alt+X", 102}
    };
    menus.push_back(m_file);

    TuiMenu m_opt;
    m_opt.title = "Opcoes";
    m_opt.x_pos = 10;
    m_opt.width = 8; // " Opcoes "
    m_opt.items = {
        {"Tema Normal", "F2", 201},
        {"Tema Invertido", "", 202},
        {"Tema Claro", "", 203},
        {"Tema Escuro", "", 204}
    };
    menus.push_back(m_opt);

    TuiMenu m_help;
    m_help.title = "Ajuda";
    m_help.x_pos = 18;
    m_help.width = 7; // " Ajuda "
    m_help.items = {
        {"Sobre o zx81ide", "F1", 301}
    };
    menus.push_back(m_help);

    return menus;
}

} // anonymous namespace

int run_tui(const std::string& initial_file) {
    setup_console();

    std::string current_file = initial_file.empty() ? "NONAME.BAS" : initial_file;
    std::string current_theme_name = core::ConfigManager::instance().get_theme();
    TuiTheme theme = get_tui_theme(current_theme_name);

    // Se arquivo foi informado pela linha de comando, abre editor de imediato
    if (!initial_file.empty()) {
        int ret = zx81ide::editor::run_text_editor(current_file, zx81ide::editor::EditorPresentation::TUI_INTEGRATED);
        if (ret == 1) {
            restore_console();
            return 0;
        }
        setup_console();
        current_theme_name = core::ConfigManager::instance().get_theme();
        theme = get_tui_theme(current_theme_name);
    }

    bool running = true;
    int current_menu = 0;
    int menu_item_idx = 0;
    bool menu_open = false;
    bool show_about = false;
    DWORD prev_btn_state = 0;

    auto menus = build_tui_menus();

    auto execute_action = [&](int action_id) {
        if (action_id == 101) { // Editor Livre
            int ret = zx81ide::editor::run_text_editor(current_file, zx81ide::editor::EditorPresentation::TUI_INTEGRATED);
            if (ret == 1) {
                running = false;
            } else {
                setup_console();
                current_theme_name = core::ConfigManager::instance().get_theme();
                theme = get_tui_theme(current_theme_name);
            }
        } else if (action_id == 102) { // Sair da TUI
            running = false;
        } else if (action_id >= 201 && action_id <= 204) { // Temas
            std::string next_theme = "normal";
            if (action_id == 202) next_theme = "invertido";
            else if (action_id == 203) next_theme = "claro";
            else if (action_id == 204) next_theme = "escuro";

            core::ConfigManager::instance().set_theme(next_theme);
            current_theme_name = next_theme;
            theme = get_tui_theme(current_theme_name);
        } else if (action_id == 301) { // Sobre
            show_about = true;
        }
    };

#ifdef _WIN32
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
#endif

    // Redesenho ultrarrapido apenas dos itens do menu dropdown (0.05ms, sem repaint de tela nem flicker)
    auto redraw_tui_dropdown_fast = [&]() {
        if (!menu_open || current_menu < 0 || current_menu >= (int)menus.size()) return;
        const auto& cur_menu = menus[current_menu];
        int drop_x = cur_menu.x_pos;
        int drop_y = 1;
        int num_items = (int)cur_menu.items.size();
        int drop_inner_w = 20;
        for (const auto& item : cur_menu.items) {
            int item_len = (int)item.label.size() + (int)item.shortcut.size() + 4;
            if (item_len > drop_inner_w) drop_inner_w = item_len;
        }
        std::ostringstream dbuf;
        for (int i = 0; i < num_items; ++i) {
            dbuf << "\x1b[" << (drop_y + 2 + i) << ";" << (drop_x + 2) << "H";
            std::string item_str = format_menu_item(cur_menu.items[i].label, cur_menu.items[i].shortcut, drop_inner_w);
            if (menu_item_idx == i) {
                dbuf << theme.drop_sel_fg << theme.drop_sel_bg << item_str;
            } else {
                dbuf << theme.drop_item_fg << theme.drop_item_bg << item_str;
            }
        }
        dbuf << Colors::RESET << Colors::HIDE_CURSOR;
        std::cout << dbuf.str() << std::flush;
    };

    int width = 80;
    int height = 25;
    bool needs_render = true;

    while (running) {
        if (needs_render) {
            get_terminal_size(width, height);

            ScreenBuffer screen(width, height);

        // -------------------------------------------------------------
        // 1. ÁREA DE DESKTOP (Linhas 1 até height - 2)
        // -------------------------------------------------------------
        for (int y = 1; y < height - 1; ++y) {
            for (int x = 0; x < width; ++x) {
                screen.put_cell(x, y, theme.desktop_char, theme.desktop_fg, theme.desktop_bg);
            }
        }

        // -------------------------------------------------------------
        // 2. BARRA DE MENU (Linha 0)
        // -------------------------------------------------------------
        for (int x = 0; x < width; ++x) {
            screen.put_cell(x, 0, " ", theme.menu_bar_fg, theme.menu_bar_bg);
        }

        for (size_t m = 0; m < menus.size(); ++m) {
            bool is_active_menu = (menu_open && (int)m == current_menu);
            std::string title_str = " " + menus[m].title + " ";
            screen.put_string(menus[m].x_pos, 0, title_str,
                (is_active_menu ? theme.menu_active_fg : theme.menu_bar_fg),
                (is_active_menu ? theme.menu_active_bg : theme.menu_bar_bg));
        }

        // -------------------------------------------------------------
        // 3. BARRA DE STATUS (Última Linha: height - 1)
        // -------------------------------------------------------------
        int status_y = height - 1;
        for (int x = 0; x < width; ++x) {
            screen.put_cell(x, status_y, " ", theme.status_fg, theme.status_bg);
        }

        std::string shortcuts = " F1:Sobre | F2:Tema | F3:Editor | F10:Menu | Alt+X:Sair ";
        screen.put_string(1, status_y, shortcuts, theme.status_fg, theme.status_bg);

        std::string version_tag = "| Tema: " + theme.name + " | v" + get_version_string();
        int version_x = width - static_cast<int>(version_tag.size()) - 1;
        if (version_x > static_cast<int>(shortcuts.size())) {
            screen.put_string(version_x, status_y, version_tag, theme.status_fg, theme.status_bg);
        }

        // -------------------------------------------------------------
        // 4. SUBMENU DROPDOWN
        // -------------------------------------------------------------
        if (menu_open && !show_about && current_menu >= 0 && current_menu < (int)menus.size()) {
            const auto& cur_menu = menus[current_menu];
            int drop_x = cur_menu.x_pos;
            int drop_y = 1;
            int num_items = (int)cur_menu.items.size();

            int drop_inner_w = 20;
            for (const auto& item : cur_menu.items) {
                int item_len = (int)item.label.size() + (int)item.shortcut.size() + 4;
                if (item_len > drop_inner_w) drop_inner_w = item_len;
            }
            int drop_w = drop_inner_w + 2;

            screen.put_string(drop_x, drop_y, "┌" + repeat_utf8("─", drop_inner_w) + "┐", theme.drop_border_fg, theme.drop_border_bg);
            for (int i = 0; i < num_items; ++i) {
                screen.put_string(drop_x, drop_y + 1 + i, "│" + std::string(drop_inner_w, ' ') + "│", theme.drop_border_fg, theme.drop_border_bg);
                std::string item_str = format_menu_item(cur_menu.items[i].label, cur_menu.items[i].shortcut, drop_inner_w);
                screen.put_string(drop_x + 1, drop_y + 1 + i, item_str,
                    (menu_item_idx == i ? theme.drop_sel_fg : theme.drop_item_fg),
                    (menu_item_idx == i ? theme.drop_sel_bg : theme.drop_item_bg));

                // Sombra lateral direita MS-DOS
                screen.put_cell(drop_x + drop_w, drop_y + 1 + i, "▒", theme.drop_shadow_fg, theme.drop_shadow_bg);
                screen.put_cell(drop_x + drop_w + 1, drop_y + 1 + i, "▒", theme.drop_shadow_fg, theme.drop_shadow_bg);
            }
            screen.put_string(drop_x, drop_y + 1 + num_items, "└" + repeat_utf8("─", drop_inner_w) + "┘", theme.drop_border_fg, theme.drop_border_bg);
            screen.put_cell(drop_x + drop_w, drop_y + 1 + num_items, "▒", theme.drop_shadow_fg, theme.drop_shadow_bg);
            screen.put_cell(drop_x + drop_w + 1, drop_y + 1 + num_items, "▒", theme.drop_shadow_fg, theme.drop_shadow_bg);

            // Sombra inferior MS-DOS
            for (int sx = 2; sx <= drop_w + 1; ++sx) {
                screen.put_cell(drop_x + sx, drop_y + 2 + num_items, "▒", theme.drop_shadow_fg, theme.drop_shadow_bg);
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

            for (int dy = 0; dy < dlg_h; ++dy) {
                for (int dx = 0; dx < dlg_w; ++dx) {
                    screen.put_cell(dlg_x + dx, dlg_y + dy, " ", theme.dlg_text_fg, theme.dlg_body_bg);
                }
            }

            screen.put_string(dlg_x, dlg_y, "╔" + repeat_utf8("═", dlg_w - 2) + "╗", theme.dlg_border_fg, theme.dlg_border_bg);
            for (int dy = 1; dy < dlg_h - 1; ++dy) {
                screen.put_cell(dlg_x, dlg_y + dy, "║", theme.dlg_border_fg, theme.dlg_border_bg);
                screen.put_cell(dlg_x + dlg_w - 1, dlg_y + dy, "║", theme.dlg_border_fg, theme.dlg_border_bg);
            }
            screen.put_string(dlg_x, dlg_y + dlg_h - 1, "╚" + repeat_utf8("═", dlg_w - 2) + "╝", theme.dlg_border_fg, theme.dlg_border_bg);

            std::string title = "[ Sobre o zx81ide ]";
            screen.put_string(dlg_x + (dlg_w - static_cast<int>(title.size())) / 2, dlg_y, title, theme.dlg_title_fg, theme.dlg_title_bg);

            screen.put_string(dlg_x + 3, dlg_y + 2,  "zx81ide - IDE para Sinclair ZX-81 & Clones", theme.dlg_text_fg, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 3,  "Linha Sinclair ZX-81, TK82/83/85, CP200, TS1000", theme.dlg_text_dim, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 5,  "Versao      : v" + get_version_string(), theme.dlg_text_fg, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 6,  "Hardware (X): " + VERSION_MAJOR_NAME, theme.dlg_text_dim, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 7,  "Jogo     (Y): " + VERSION_GAME_CODENAME, theme.dlg_text_dim, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 8,  "Build    (Z): " + VERSION_BUILD_SUBTITLE, theme.dlg_text_dim, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 9,  "Tema Ativo  : " + theme.name + " (SQLite 3)", theme.dlg_text_fg, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 11, "Autor       : Wilson \"Barney\" Pilon", theme.dlg_text_fg, theme.dlg_body_bg);
            screen.put_string(dlg_x + 3, dlg_y + 12, "Inspiracao  : fwMSX (github.com/wilsonpilon/fwMSX)", theme.dlg_text_dim, theme.dlg_body_bg);

            std::string ok_btn = " [  OK  ] ";
            screen.put_string(dlg_x + (dlg_w - static_cast<int>(ok_btn.size())) / 2, dlg_y + 14, ok_btn, theme.dlg_btn_fg, theme.dlg_btn_bg);

            // Sombra do diálogo MS-DOS
            for (int dy = 1; dy <= dlg_h; ++dy) {
                screen.put_cell(dlg_x + dlg_w, dlg_y + dy, "▒", theme.dlg_shadow_fg, theme.dlg_shadow_bg);
                screen.put_cell(dlg_x + dlg_w + 1, dlg_y + dy, "▒", theme.dlg_shadow_fg, theme.dlg_shadow_bg);
            }
            for (int dx = 2; dx <= dlg_w + 1; ++dx) {
                screen.put_cell(dlg_x + dx, dlg_y + dlg_h, "▒", theme.dlg_shadow_fg, theme.dlg_shadow_bg);
            }
        }

            screen.render();
            needs_render = false;
        }

        // -------------------------------------------------------------
        // 6. PROCESSAMENTO DE ENTRADA (TECLADO E MOUSE COM READCONSOLEINPUT)
        // -------------------------------------------------------------
#ifdef _WIN32
        DWORD wait_res = WaitForSingleObject(hIn, 40);
        if (wait_res == WAIT_TIMEOUT) {
            int cur_w = 80, cur_h = 25;
            get_terminal_size(cur_w, cur_h);
            if (cur_w != width || cur_h != height) {
                needs_render = true;
            }
            continue;
        }

        INPUT_RECORD ir[16];
        DWORD num_events = 0;
        if (!ReadConsoleInputW(hIn, ir, 16, &num_events) || num_events == 0) {
            continue;
        }

        for (DWORD e = 0; e < num_events; ++e) {
            auto& rec = ir[e];

            // ---------------------------------------------------------
            // EVENTOS DE REDIMENSIONAMENTO DE JANELA CONSOLE
            // ---------------------------------------------------------
            if (rec.EventType == WINDOW_BUFFER_SIZE_EVENT) {
                needs_render = true;
                continue;
            }

            // ---------------------------------------------------------
            // EVENTOS DE MOUSE
            // ---------------------------------------------------------
            if (rec.EventType == MOUSE_EVENT) {
                COORD mpos = rec.Event.MouseEvent.dwMousePosition;
                DWORD btn = rec.Event.MouseEvent.dwButtonState;
                DWORD flags = rec.Event.MouseEvent.dwEventFlags;

                bool left_clicked = (btn & FROM_LEFT_1ST_BUTTON_PRESSED) && !(prev_btn_state & FROM_LEFT_1ST_BUTTON_PRESSED);

                // 1. Movimentacao do Mouse (hover sobre menus e itens)
                if (flags & MOUSE_MOVED) {
                    if (menu_open) {
                        if (mpos.Y == 0) {
                            for (size_t m = 0; m < menus.size(); ++m) {
                                if (mpos.X >= menus[m].x_pos && mpos.X < menus[m].x_pos + menus[m].width) {
                                    if (current_menu != (int)m) {
                                        current_menu = (int)m;
                                        menu_item_idx = 0;
                                        needs_render = true;
                                    }
                                    break;
                                }
                            }
                        } else if (current_menu >= 0 && current_menu < (int)menus.size()) {
                            const auto& cur_menu = menus[current_menu];
                            int drop_x = cur_menu.x_pos;
                            int drop_y = 1;
                            int num_items = (int)cur_menu.items.size();
                            int drop_inner_w = 20;
                            for (const auto& item : cur_menu.items) {
                                int item_len = (int)item.label.size() + (int)item.shortcut.size() + 4;
                                if (item_len > drop_inner_w) drop_inner_w = item_len;
                            }
                            int drop_w = drop_inner_w + 2;

                            if (mpos.Y >= drop_y + 1 && mpos.Y <= drop_y + num_items &&
                                mpos.X >= drop_x + 1 && mpos.X <= drop_x + drop_w - 1) {
                                int new_item = mpos.Y - (drop_y + 1);
                                if (new_item != menu_item_idx) {
                                    menu_item_idx = new_item;
                                    redraw_tui_dropdown_fast();
                                }
                            }
                        }
                    }
                }

                // 2. Clique com Botao Esquerdo (apenas no pressionamento inicial)
                if (left_clicked) {
                    needs_render = true;
                    if (show_about) {
                        show_about = false;
                        prev_btn_state = btn;
                        break;
                    }

                    // Clique na Barra de Menus Superior (Linha 0)
                    if (mpos.Y == 0) {
                        bool clicked_menu = false;
                        for (size_t m = 0; m < menus.size(); ++m) {
                            if (mpos.X >= menus[m].x_pos && mpos.X < menus[m].x_pos + menus[m].width) {
                                if (menu_open && current_menu == (int)m) {
                                    menu_open = false;
                                } else {
                                    current_menu = (int)m;
                                    menu_item_idx = 0;
                                    menu_open = true;
                                }
                                clicked_menu = true;
                                break;
                            }
                        }
                        if (!clicked_menu) {
                            menu_open = false;
                        }
                        prev_btn_state = btn;
                        break;
                    }

                    // Clique no Submenu Dropdown aberto
                    if (menu_open && current_menu >= 0 && current_menu < (int)menus.size()) {
                        const auto& cur_menu = menus[current_menu];
                        int drop_x = cur_menu.x_pos;
                        int drop_y = 1;
                        int num_items = (int)cur_menu.items.size();
                        int drop_inner_w = 20;
                        for (const auto& item : cur_menu.items) {
                            int item_len = (int)item.label.size() + (int)item.shortcut.size() + 4;
                            if (item_len > drop_inner_w) drop_inner_w = item_len;
                        }
                        int drop_w = drop_inner_w + 2;

                        if (mpos.Y >= drop_y + 1 && mpos.Y <= drop_y + num_items &&
                            mpos.X >= drop_x + 1 && mpos.X <= drop_x + drop_w - 1) {
                            int clicked_idx = mpos.Y - (drop_y + 1);
                            if (clicked_idx >= 0 && clicked_idx < num_items) {
                                int act_id = cur_menu.items[clicked_idx].id;
                                menu_open = false;
                                execute_action(act_id);
                                prev_btn_state = btn;
                                break;
                            }
                        } else {
                            menu_open = false;
                            prev_btn_state = btn;
                            break;
                        }
                    }
                }

                prev_btn_state = btn;
                continue;
            }

            // ---------------------------------------------------------
            // EVENTOS DE TECLADO
            // ---------------------------------------------------------
            if (rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown) {
                needs_render = true;
                WORD vk = rec.Event.KeyEvent.wVirtualKeyCode;
                WCHAR wc = rec.Event.KeyEvent.uChar.UnicodeChar;
                DWORD ctrl_state = rec.Event.KeyEvent.dwControlKeyState;

                bool is_alt = (ctrl_state & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0;
                bool is_ctrl = (ctrl_state & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;

                if (show_about) {
                    if (vk == VK_ESCAPE || vk == VK_RETURN || vk == VK_SPACE) {
                        show_about = false;
                    }
                    break;
                }

                // F1: Sobre
                if (vk == VK_F1) {
                    execute_action(301);
                    break;
                }

                // F2: Alternar tema
                if (vk == VK_F2) {
                    std::string next_theme = "normal";
                    if (current_theme_name == "normal") next_theme = "invertido";
                    else if (current_theme_name == "invertido") next_theme = "claro";
                    else if (current_theme_name == "claro") next_theme = "escuro";
                    else next_theme = "normal";

                    core::ConfigManager::instance().set_theme(next_theme);
                    current_theme_name = next_theme;
                    theme = get_tui_theme(current_theme_name);
                    break;
                }

                // F3: Editor Livre
                if (vk == VK_F3) {
                    menu_open = false;
                    execute_action(101);
                    break;
                }

                // F10: Alternar menu bar
                if (vk == VK_F10) {
                    menu_open = !menu_open;
                    menu_item_idx = 0;
                    break;
                }

                // ESC: Fecha menu ou sai da TUI
                if (vk == VK_ESCAPE) {
                    if (menu_open) {
                        menu_open = false;
                    } else {
                        running = false;
                    }
                    break;
                }

                // Alt+X ou Ctrl+Q: Sair
                if ((is_alt && (vk == 'X' || vk == 'x')) || (is_ctrl && (vk == 'Q' || vk == 'q'))) {
                    running = false;
                    break;
                }

                // Alt+A: Arquivo
                if (is_alt && (vk == 'A' || vk == 'a')) {
                    current_menu = 0;
                    menu_open = true;
                    menu_item_idx = 0;
                    break;
                }

                // Alt+O: Opcoes
                if (is_alt && (vk == 'O' || vk == 'o')) {
                    current_menu = 1;
                    menu_open = true;
                    menu_item_idx = 0;
                    break;
                }

                // Alt+J ou Alt+H: Ajuda
                if (is_alt && (vk == 'J' || vk == 'j' || vk == 'H' || vk == 'h')) {
                    current_menu = 2;
                    menu_open = true;
                    menu_item_idx = 0;
                    break;
                }

                // Setas
                if (vk == VK_LEFT) {
                    current_menu = (current_menu + (int)menus.size() - 1) % (int)menus.size();
                    menu_item_idx = 0;
                    break;
                } else if (vk == VK_RIGHT) {
                    current_menu = (current_menu + 1) % (int)menus.size();
                    menu_item_idx = 0;
                    break;
                } else if (vk == VK_DOWN) {
                    if (!menu_open) {
                        menu_open = true;
                        menu_item_idx = 0;
                        needs_render = true;
                    } else {
                        menu_item_idx = (menu_item_idx + 1) % (int)menus[current_menu].items.size();
                        redraw_tui_dropdown_fast();
                        needs_render = false;
                    }
                    break;
                } else if (vk == VK_UP) {
                    if (menu_open) {
                        menu_item_idx = (menu_item_idx + (int)menus[current_menu].items.size() - 1) % (int)menus[current_menu].items.size();
                        redraw_tui_dropdown_fast();
                        needs_render = false;
                    }
                    break;
                }

                // Enter ou Espaco
                if (vk == VK_RETURN || vk == VK_SPACE) {
                    if (!menu_open) {
                        menu_open = true;
                        menu_item_idx = 0;
                    } else {
                        int act_id = menus[current_menu].items[menu_item_idx].id;
                        menu_open = false;
                        execute_action(act_id);
                    }
                    break;
                }

                // Teclas diretas
                if (!menu_open) {
                    if (wc == 'e' || wc == 'E') {
                        execute_action(101);
                        break;
                    } else if (wc == 't' || wc == 'T') {
                        std::string next_theme = "normal";
                        if (current_theme_name == "normal") next_theme = "invertido";
                        else if (current_theme_name == "invertido") next_theme = "claro";
                        else if (current_theme_name == "claro") next_theme = "escuro";
                        else next_theme = "normal";

                        core::ConfigManager::instance().set_theme(next_theme);
                        current_theme_name = next_theme;
                        theme = get_tui_theme(current_theme_name);
                        break;
                    }
                }
            }
        }
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
