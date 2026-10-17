#include "editor/text_editor.h"
#include "core/version.h"
#include "core/config_manager.h"
#include "compiler/zxtext2p.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cctype>

#ifdef _WIN32
#include <windows.h>
#endif

namespace zx81ide {
namespace editor {

namespace {

// Paleta Estrita Sinclair ZX-81
namespace Colors {
    const std::string RESET         = "\x1b[0m";
    const std::string HIDE_CURSOR   = "\x1b[?25l";
    const std::string SHOW_CURSOR   = "\x1b[?25h";
    const std::string HOME          = "\x1b[H";

    // Textos
    const std::string FG_BLACK      = "\x1b[30m";
    const std::string FG_WHITE      = "\x1b[97m";
    const std::string FG_GRAY_LIGHT = "\x1b[37m";
    const std::string FG_GRAY_DARK  = "\x1b[90m";

    // Fundos
    const std::string BG_BLACK      = "\x1b[40m";
    const std::string BG_WHITE      = "\x1b[107m";
    const std::string BG_GRAY_LIGHT = "\x1b[47m";
    const std::string BG_GRAY_DARK  = "\x1b[100m";
}

struct EditorTheme {
    std::string desktop_char;
    std::string desktop_fg;
    std::string desktop_bg;

    // Moldura da janela (dupla)
    std::string frame_fg;
    std::string frame_bg;

    // Barra de titulo e botoes
    std::string title_fg;
    std::string title_bg;
    std::string btn_fg;
    std::string btn_bg;

    // Interior da janela de edicao
    std::string text_fg;
    std::string text_bg;

    // Destaque de Sintaxe (Syntax Highlighting)
    std::string syn_keyword_fg;
    std::string syn_keyword_bg;
    std::string syn_comment_fg;
    std::string syn_comment_bg;
    std::string syn_string_fg;
    std::string syn_string_bg;
    std::string syn_number_fg;
    std::string syn_number_bg;
    std::string syn_label_fg;
    std::string syn_label_bg;
    std::string syn_escape_fg;
    std::string syn_escape_bg;
    std::string syn_operator_fg;
    std::string syn_operator_bg;

    // Barras de rolagem (vertical e horizontal)
    std::string scroll_track_fg;
    std::string scroll_track_bg;
    std::string scroll_thumb_fg;
    std::string scroll_thumb_bg;
    std::string scroll_arrow_fg;
    std::string scroll_arrow_bg;

    // Barra de status inferior
    std::string status_fg;
    std::string status_bg;

    // Barra de Menus Superior (Linha 0)
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

    // Dialogo Salvar / Mensagens / Modais
    std::string dlg_border_fg;
    std::string dlg_border_bg;
    std::string dlg_body_bg;
    std::string dlg_text_fg;
    std::string dlg_btn_fg;
    std::string dlg_btn_bg;
    std::string dlg_btn_sel_fg;
    std::string dlg_btn_sel_bg;
};

EditorTheme get_editor_theme(const std::string& theme_name) {
    std::string name = theme_name;
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    EditorTheme th;
    if (name == "invertido") {
        th.desktop_char      = "░";
        th.desktop_fg        = Colors::FG_BLACK;
        th.desktop_bg        = Colors::BG_GRAY_DARK;

        th.frame_fg          = Colors::FG_BLACK;
        th.frame_bg          = Colors::BG_GRAY_LIGHT;
        th.title_fg          = Colors::FG_BLACK;
        th.title_bg          = Colors::BG_GRAY_LIGHT;
        th.btn_fg            = Colors::FG_WHITE;
        th.btn_bg            = Colors::BG_BLACK;

        th.text_fg           = Colors::FG_BLACK;
        th.text_bg           = Colors::BG_GRAY_LIGHT;

        th.syn_keyword_fg    = "\x1b[1;30m";
        th.syn_keyword_bg    = Colors::BG_GRAY_LIGHT;
        th.syn_comment_fg    = Colors::FG_GRAY_DARK;
        th.syn_comment_bg    = Colors::BG_GRAY_LIGHT;
        th.syn_string_fg     = Colors::FG_WHITE;
        th.syn_string_bg     = Colors::BG_BLACK;
        th.syn_number_fg     = Colors::FG_BLACK;
        th.syn_number_bg     = Colors::BG_GRAY_LIGHT;
        th.syn_label_fg      = "\x1b[1;30m";
        th.syn_label_bg      = Colors::BG_GRAY_LIGHT;
        th.syn_escape_fg     = Colors::FG_WHITE;
        th.syn_escape_bg     = Colors::BG_BLACK;
        th.syn_operator_fg   = Colors::FG_BLACK;
        th.syn_operator_bg   = Colors::BG_GRAY_LIGHT;

        th.scroll_track_fg   = Colors::FG_GRAY_DARK;
        th.scroll_track_bg   = Colors::BG_GRAY_LIGHT;
        th.scroll_thumb_fg   = Colors::FG_BLACK;
        th.scroll_thumb_bg   = Colors::BG_GRAY_LIGHT;
        th.scroll_arrow_fg   = Colors::FG_BLACK;
        th.scroll_arrow_bg   = Colors::BG_GRAY_LIGHT;

        th.status_fg         = Colors::FG_WHITE;
        th.status_bg         = Colors::BG_BLACK;

        th.menu_bar_fg       = Colors::FG_BLACK;
        th.menu_bar_bg       = Colors::BG_GRAY_LIGHT;
        th.menu_active_fg    = Colors::FG_WHITE;
        th.menu_active_bg    = Colors::BG_BLACK;

        th.drop_border_fg    = Colors::FG_BLACK;
        th.drop_border_bg    = Colors::BG_GRAY_LIGHT;
        th.drop_item_fg      = Colors::FG_BLACK;
        th.drop_item_bg      = Colors::BG_GRAY_LIGHT;
        th.drop_sel_fg       = Colors::FG_WHITE;
        th.drop_sel_bg       = Colors::BG_BLACK;
        th.drop_shadow_fg    = Colors::FG_BLACK;
        th.drop_shadow_bg    = Colors::BG_GRAY_DARK;

        th.dlg_border_fg     = Colors::FG_BLACK;
        th.dlg_border_bg     = Colors::BG_GRAY_LIGHT;
        th.dlg_body_bg       = Colors::BG_GRAY_LIGHT;
        th.dlg_text_fg       = Colors::FG_BLACK;
        th.dlg_btn_fg        = Colors::FG_BLACK;
        th.dlg_btn_bg        = Colors::BG_GRAY_LIGHT;
        th.dlg_btn_sel_fg    = Colors::FG_WHITE;
        th.dlg_btn_sel_bg    = Colors::BG_BLACK;
    } else if (name == "escuro") {
        th.desktop_char      = "░";
        th.desktop_fg        = Colors::FG_GRAY_DARK;
        th.desktop_bg        = Colors::BG_BLACK;

        th.frame_fg          = Colors::FG_WHITE;
        th.frame_bg          = Colors::BG_BLACK;
        th.title_fg          = Colors::FG_WHITE;
        th.title_bg          = Colors::BG_BLACK;
        th.btn_fg            = Colors::FG_BLACK;
        th.btn_bg            = Colors::BG_WHITE;

        th.text_fg           = Colors::FG_WHITE;
        th.text_bg           = Colors::BG_BLACK;

        th.syn_keyword_fg    = "\x1b[1;97m";
        th.syn_keyword_bg    = Colors::BG_BLACK;
        th.syn_comment_fg    = Colors::FG_GRAY_DARK;
        th.syn_comment_bg    = Colors::BG_BLACK;
        th.syn_string_fg     = Colors::FG_GRAY_LIGHT;
        th.syn_string_bg     = Colors::BG_BLACK;
        th.syn_number_fg     = Colors::FG_WHITE;
        th.syn_number_bg     = Colors::BG_BLACK;
        th.syn_label_fg      = "\x1b[1;37m";
        th.syn_label_bg      = Colors::BG_BLACK;
        th.syn_escape_fg     = Colors::FG_BLACK;
        th.syn_escape_bg     = Colors::BG_WHITE;
        th.syn_operator_fg   = Colors::FG_WHITE;
        th.syn_operator_bg   = Colors::BG_BLACK;

        th.scroll_track_fg   = Colors::FG_GRAY_DARK;
        th.scroll_track_bg   = Colors::BG_BLACK;
        th.scroll_thumb_fg   = Colors::FG_WHITE;
        th.scroll_thumb_bg   = Colors::BG_BLACK;
        th.scroll_arrow_fg   = Colors::FG_WHITE;
        th.scroll_arrow_bg   = Colors::BG_BLACK;

        th.status_fg         = Colors::FG_WHITE;
        th.status_bg         = Colors::BG_BLACK;

        th.menu_bar_fg       = Colors::FG_WHITE;
        th.menu_bar_bg       = Colors::BG_BLACK;
        th.menu_active_fg    = Colors::FG_BLACK;
        th.menu_active_bg    = Colors::BG_WHITE;

        th.drop_border_fg    = Colors::FG_WHITE;
        th.drop_border_bg    = Colors::BG_BLACK;
        th.drop_item_fg      = Colors::FG_WHITE;
        th.drop_item_bg      = Colors::BG_BLACK;
        th.drop_sel_fg       = Colors::FG_BLACK;
        th.drop_sel_bg       = Colors::BG_WHITE;
        th.drop_shadow_fg    = Colors::FG_GRAY_DARK;
        th.drop_shadow_bg    = Colors::BG_BLACK;

        th.dlg_border_fg     = Colors::FG_WHITE;
        th.dlg_border_bg     = Colors::BG_BLACK;
        th.dlg_body_bg       = Colors::BG_BLACK;
        th.dlg_text_fg       = Colors::FG_WHITE;
        th.dlg_btn_fg        = Colors::FG_WHITE;
        th.dlg_btn_bg        = Colors::BG_BLACK;
        th.dlg_btn_sel_fg    = Colors::FG_BLACK;
        th.dlg_btn_sel_bg    = Colors::BG_WHITE;
    } else if (name == "claro") {
        th.desktop_char      = "░";
        th.desktop_fg        = Colors::FG_GRAY_DARK;
        th.desktop_bg        = Colors::BG_GRAY_LIGHT;

        th.frame_fg          = Colors::FG_BLACK;
        th.frame_bg          = Colors::BG_WHITE;
        th.title_fg          = Colors::FG_BLACK;
        th.title_bg          = Colors::BG_WHITE;
        th.btn_fg            = Colors::FG_WHITE;
        th.btn_bg            = Colors::BG_GRAY_DARK;

        th.text_fg           = Colors::FG_BLACK;
        th.text_bg           = Colors::BG_WHITE;

        th.syn_keyword_fg    = "\x1b[1;30m";
        th.syn_keyword_bg    = Colors::BG_WHITE;
        th.syn_comment_fg    = Colors::FG_GRAY_DARK;
        th.syn_comment_bg    = Colors::BG_WHITE;
        th.syn_string_fg     = Colors::FG_GRAY_DARK;
        th.syn_string_bg     = Colors::BG_WHITE;
        th.syn_number_fg     = Colors::FG_BLACK;
        th.syn_number_bg     = Colors::BG_WHITE;
        th.syn_label_fg      = "\x1b[1;30m";
        th.syn_label_bg      = Colors::BG_WHITE;
        th.syn_escape_fg     = Colors::FG_WHITE;
        th.syn_escape_bg     = Colors::BG_GRAY_DARK;
        th.syn_operator_fg   = Colors::FG_BLACK;
        th.syn_operator_bg   = Colors::BG_WHITE;

        th.scroll_track_fg   = Colors::FG_GRAY_DARK;
        th.scroll_track_bg   = Colors::BG_WHITE;
        th.scroll_thumb_fg   = Colors::FG_GRAY_DARK;
        th.scroll_thumb_bg   = Colors::BG_WHITE;
        th.scroll_arrow_fg   = Colors::FG_BLACK;
        th.scroll_arrow_bg   = Colors::BG_WHITE;

        th.status_fg         = Colors::FG_BLACK;
        th.status_bg         = Colors::BG_WHITE;

        th.menu_bar_fg       = Colors::FG_BLACK;
        th.menu_bar_bg       = Colors::BG_WHITE;
        th.menu_active_fg    = Colors::FG_WHITE;
        th.menu_active_bg    = Colors::BG_GRAY_DARK;

        th.drop_border_fg    = Colors::FG_BLACK;
        th.drop_border_bg    = Colors::BG_WHITE;
        th.drop_item_fg      = Colors::FG_BLACK;
        th.drop_item_bg      = Colors::BG_WHITE;
        th.drop_sel_fg       = Colors::FG_WHITE;
        th.drop_sel_bg       = Colors::BG_GRAY_DARK;
        th.drop_shadow_fg    = Colors::FG_GRAY_DARK;
        th.drop_shadow_bg    = Colors::BG_GRAY_LIGHT;

        th.dlg_border_fg     = Colors::FG_BLACK;
        th.dlg_border_bg     = Colors::BG_WHITE;
        th.dlg_body_bg       = Colors::BG_WHITE;
        th.dlg_text_fg       = Colors::FG_BLACK;
        th.dlg_btn_fg        = Colors::FG_BLACK;
        th.dlg_btn_bg        = Colors::BG_WHITE;
        th.dlg_btn_sel_fg    = Colors::FG_WHITE;
        th.dlg_btn_sel_bg    = Colors::BG_GRAY_DARK;
    } else {
        // Padrao "normal" (Sinclair ZX-81 Original)
        th.desktop_char      = "░";
        th.desktop_fg        = Colors::FG_GRAY_DARK;
        th.desktop_bg        = Colors::BG_BLACK;

        th.frame_fg          = Colors::FG_WHITE;
        th.frame_bg          = Colors::BG_BLACK;
        th.title_fg          = Colors::FG_WHITE;
        th.title_bg          = Colors::BG_BLACK;
        th.btn_fg            = Colors::FG_BLACK;
        th.btn_bg            = Colors::BG_WHITE;

        th.text_fg           = Colors::FG_WHITE;
        th.text_bg           = Colors::BG_BLACK;

        th.syn_keyword_fg    = "\x1b[1;97m";
        th.syn_keyword_bg    = Colors::BG_BLACK;
        th.syn_comment_fg    = Colors::FG_GRAY_DARK;
        th.syn_comment_bg    = Colors::BG_BLACK;
        th.syn_string_fg     = Colors::FG_GRAY_LIGHT;
        th.syn_string_bg     = Colors::BG_BLACK;
        th.syn_number_fg     = Colors::FG_WHITE;
        th.syn_number_bg     = Colors::BG_BLACK;
        th.syn_label_fg      = "\x1b[1;37m";
        th.syn_label_bg      = Colors::BG_BLACK;
        th.syn_escape_fg     = Colors::FG_BLACK;
        th.syn_escape_bg     = Colors::BG_WHITE;
        th.syn_operator_fg   = Colors::FG_WHITE;
        th.syn_operator_bg   = Colors::BG_BLACK;

        th.scroll_track_fg   = Colors::FG_GRAY_DARK;
        th.scroll_track_bg   = Colors::BG_BLACK;
        th.scroll_thumb_fg   = Colors::FG_WHITE;
        th.scroll_thumb_bg   = Colors::BG_BLACK;
        th.scroll_arrow_fg   = Colors::FG_WHITE;
        th.scroll_arrow_bg   = Colors::BG_BLACK;

        th.status_fg         = Colors::FG_BLACK;
        th.status_bg         = Colors::BG_GRAY_LIGHT;

        th.menu_bar_fg       = Colors::FG_WHITE;
        th.menu_bar_bg       = Colors::BG_BLACK;
        th.menu_active_fg    = Colors::FG_BLACK;
        th.menu_active_bg    = Colors::BG_WHITE;

        th.drop_border_fg    = Colors::FG_WHITE;
        th.drop_border_bg    = Colors::BG_BLACK;
        th.drop_item_fg      = Colors::FG_WHITE;
        th.drop_item_bg      = Colors::BG_BLACK;
        th.drop_sel_fg       = Colors::FG_BLACK;
        th.drop_sel_bg       = Colors::BG_WHITE;
        th.drop_shadow_fg    = Colors::FG_GRAY_DARK;
        th.drop_shadow_bg    = Colors::BG_BLACK;

        th.dlg_border_fg     = Colors::FG_WHITE;
        th.dlg_border_bg     = Colors::BG_BLACK;
        th.dlg_body_bg       = Colors::BG_BLACK;
        th.dlg_text_fg       = Colors::FG_WHITE;
        th.dlg_btn_fg        = Colors::FG_WHITE;
        th.dlg_btn_bg        = Colors::BG_BLACK;
        th.dlg_btn_sel_fg    = Colors::FG_BLACK;
        th.dlg_btn_sel_bg    = Colors::BG_WHITE;
    }

    return th;
}

std::string repeat_utf8(const std::string& ch, int count) {
    std::string result;
    result.reserve(ch.size() * count);
    for (int i = 0; i < count; ++i) {
        result += ch;
    }
    return result;
}

int get_visible_width(const std::string& str) {
    int w = 0;
    size_t i = 0;
    while (i < str.size()) {
        if (str[i] == '\x1b') {
            i++;
            if (i < str.size() && str[i] == '[') {
                i++;
                while (i < str.size() && (str[i] < '@' || str[i] > '~')) {
                    i++;
                }
                if (i < str.size()) i++;
            }
            continue;
        }
        unsigned char c = (unsigned char)str[i];
        if ((c & 0xC0) != 0x80) {
            w++;
        }
        i++;
    }
    return w;
}

std::string format_menu_item(const std::string& label, const std::string& shortcut, int inner_width) {
    std::string text = " " + label;
    int vis_text = get_visible_width(text);
    int vis_sc = get_visible_width(shortcut);
    int spaces = inner_width - vis_text - vis_sc - 1;
    if (spaces < 1) spaces = 1;
    text += std::string(spaces, ' ') + shortcut + " ";
    return text;
}

std::string invert_zx_string(const std::string& input) {
    if (input.empty()) return "";

    // 1. Verifica se a string ja esta completamente invertida com '%'
    size_t normal_count = 0;
    size_t inv_count = 0;

    for (size_t i = 0; i < input.size(); ) {
        if (input[i] == '%') {
            inv_count++;
            i += 2;
        } else if (input[i] == '\\') {
            if (i + 1 < input.size() && input[i + 1] == '"') {
                normal_count++;
                i += 2;
            } else if (i + 2 < input.size()) {
                normal_count++;
                i += 3;
            } else {
                normal_count++;
                i++;
            }
        } else {
            normal_count++;
            i++;
        }
    }

    // Se todos os caracteres eram invertidos com '%', remove o prefixo '%' (desinverte)
    if (inv_count > 0 && normal_count == 0) {
        std::string uninv = "";
        for (size_t i = 0; i < input.size(); ) {
            if (input[i] == '%') {
                if (i + 1 < input.size()) {
                    uninv += input[i + 1];
                    i += 2;
                } else {
                    i++;
                }
            } else {
                uninv += input[i];
                i++;
            }
        }
        return uninv;
    }

    // Caso contrario, insere '%' antes de cada caractere comum
    std::string result = "";
    for (size_t i = 0; i < input.size(); ) {
        if (input[i] == '%') {
            result += input[i];
            if (i + 1 < input.size()) {
                result += input[i + 1];
                i += 2;
            } else {
                i++;
            }
        } else if (input[i] == '\\') {
            if (i + 1 < input.size() && input[i + 1] == '"') {
                result += input.substr(i, 2);
                i += 2;
            } else if (i + 2 < input.size()) {
                result += input.substr(i, 3);
                i += 3;
            } else {
                result += input[i];
                i++;
            }
        } else {
            result += '%';
            result += input[i];
            i++;
        }
    }
    return result;
}

inline void render_dialog_top(std::ostringstream& buf, int dlg_x, int dlg_y, int dlg_w, const std::string& title, const EditorTheme& theme) {
    buf << "\x1b[" << (dlg_y + 1) << ";" << (dlg_x + 1) << "H";
    buf << theme.dlg_border_fg << theme.dlg_border_bg << "╔";
    int inner_w = dlg_w - 2;
    int title_w = get_visible_width(title);
    if (title_w > 0) {
        int total_eq = std::max(0, inner_w - title_w);
        int left_eq = total_eq / 2;
        int right_eq = total_eq - left_eq;
        buf << repeat_utf8("═", left_eq) << title << repeat_utf8("═", right_eq);
    } else {
        buf << repeat_utf8("═", inner_w);
    }
    buf << "╗";
}

inline void render_dialog_row(std::ostringstream& buf, int dlg_x, int dlg_y, int dlg_w, const std::string& colored_content, const EditorTheme& theme, bool shadow = true) {
    buf << "\x1b[" << (dlg_y + 1) << ";" << (dlg_x + 1) << "H";
    int vis_w = get_visible_width(colored_content);
    int inner_w = dlg_w - 2;
    int pad = std::max(0, inner_w - vis_w);
    buf << theme.dlg_border_fg << theme.dlg_border_bg << "║"
        << colored_content
        << theme.dlg_body_bg << std::string(pad, ' ')
        << theme.dlg_border_fg << theme.dlg_border_bg << "║";
    if (shadow) {
        buf << theme.drop_shadow_fg << theme.drop_shadow_bg << "▒";
    }
}

inline void render_dialog_bottom(std::ostringstream& buf, int dlg_x, int dlg_y, int dlg_w, const EditorTheme& theme, bool shadow = true) {
    buf << "\x1b[" << (dlg_y + 1) << ";" << (dlg_x + 1) << "H";
    buf << theme.dlg_border_fg << theme.dlg_border_bg << "╚" << repeat_utf8("═", dlg_w - 2) << "╝";
    if (shadow) {
        buf << theme.drop_shadow_fg << theme.drop_shadow_bg << "▒";
        buf << "\x1b[" << (dlg_y + 2) << ";" << (dlg_x + 2) << "H";
        buf << theme.drop_shadow_fg << theme.drop_shadow_bg << repeat_utf8("▒", dlg_w);
    }
    buf << Colors::RESET;
}

struct MenuItem {
    std::string label;
    std::string shortcut;
    int id;
};

struct MenuDef {
    std::string title;
    int x_pos;
    int width;
    std::vector<MenuItem> items;
};

std::vector<MenuDef> build_editor_menus(EditorPresentation presentation = EditorPresentation::CLI_STANDALONE) {
    std::vector<MenuDef> menus;
    
    // Arquivo
    MenuDef m_file;
    m_file.title = "Arquivo";
    m_file.x_pos = 1;
    m_file.width = 9;
    if (presentation == EditorPresentation::TUI_INTEGRATED) {
        m_file.items = {
            { "Novo",              "Ctrl+N",  101 },
            { "Salvar .BAS",       "F2",      102 },
            { "Fechar Janela",     "Ctrl+F4", 104 },
            { "Sair da TUI",       "Alt+X",   103 }
        };
    } else {
        m_file.items = {
            { "Novo",              "Ctrl+N",  101 },
            { "Salvar .BAS",       "F2",      102 },
            { "Sair",              "Alt+X",   103 }
        };
    }
    menus.push_back(m_file);

    // Editar
    MenuDef m_edit;
    m_edit.title = "Editar";
    m_edit.x_pos = 11;
    m_edit.width = 8;
    m_edit.items = {
        { "Caracteres Graficos...", "F4",     201 },
        { "Inverter Video",         "Ctrl+I", 202 }
    };
    menus.push_back(m_edit);

    // Compilar
    MenuDef m_comp;
    m_comp.title = "Compilar";
    m_comp.x_pos = 20;
    m_comp.width = 10;
    m_comp.items = {
        { "Compilar para .P (ZX-81)", "F9", 301 },
        { "Opcoes do Compilador...",  "F7", 302 }
    };
    menus.push_back(m_comp);

    // Opcoes
    MenuDef m_opt;
    m_opt.title = "Opcoes";
    m_opt.x_pos = 31;
    m_opt.width = 8;
    m_opt.items = {
        { "Tema: Normal",    "1", 401 },
        { "Tema: Invertido", "2", 402 },
        { "Tema: Claro",     "3", 403 },
        { "Tema: Escuro",    "4", 404 }
    };
    menus.push_back(m_opt);

    // Ajuda
    MenuDef m_help;
    m_help.title = "Ajuda";
    m_help.x_pos = 40;
    m_help.width = 7;
    m_help.items = {
        { "Ajuda do Editor", "F1", 501 },
        { "Sobre o zx81ide...", "", 502 }
    };
    menus.push_back(m_help);

    return menus;
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
}

std::string extract_file_name(const std::string& path) {
    size_t last_slash = path.find_last_of("\\/");
    if (last_slash != std::string::npos) {
        return path.substr(last_slash + 1);
    }
    return path;
}

std::string expand_tabs(const std::string& input, int tab_size = 4) {
    std::string output;
    for (char c : input) {
        if (c == '\t') {
            int spaces = tab_size - (output.size() % tab_size);
            output.append(spaces, ' ');
        } else {
            output.push_back(c);
        }
    }
    return output;
}

// Enumeracao das categorias de realce de sintaxe
enum class SyntaxTag : uint8_t {
    NORMAL,
    LINENUM,
    KEYWORD,
    COMMENT,
    STRING,
    NUMBER,
    LABEL,
    ESCAPE_GRAPHIC,
    OPERATOR
};

std::string get_syntax_color(SyntaxTag tag, const EditorTheme& th) {
    switch (tag) {
        case SyntaxTag::KEYWORD:        return th.syn_keyword_fg + th.syn_keyword_bg;
        case SyntaxTag::COMMENT:        return th.syn_comment_fg + th.syn_comment_bg;
        case SyntaxTag::STRING:         return th.syn_string_fg + th.syn_string_bg;
        case SyntaxTag::NUMBER:         return th.syn_number_fg + th.syn_number_bg;
        case SyntaxTag::LINENUM:        return th.syn_number_fg + th.syn_number_bg;
        case SyntaxTag::LABEL:          return th.syn_label_fg + th.syn_label_bg;
        case SyntaxTag::ESCAPE_GRAPHIC: return th.syn_escape_fg + th.syn_escape_bg;
        case SyntaxTag::OPERATOR:       return th.syn_operator_fg + th.syn_operator_bg;
        default:                        return th.text_fg + th.text_bg;
    }
}

// Analisa a linha e retorna exatamente inner_w caracteres visiveis com escapes ANSI de cores
std::string render_editor_line(const std::string& raw_line, int scroll_col, int inner_w, const EditorTheme& theme, int sel_c_start = -1, int sel_c_end = -1) {
    std::string expanded = expand_tabs(raw_line);
    size_t N = expanded.size();
    std::vector<SyntaxTag> tags(N, SyntaxTag::NORMAL);

    // 1. Linha de comentario com '#'
    size_t first_non_space = expanded.find_first_not_of(" \t");
    if (first_non_space != std::string::npos && expanded[first_non_space] == '#') {
        for (size_t i = first_non_space; i < N; ++i) {
            tags[i] = SyntaxTag::COMMENT;
        }
    } else {
        // 2. Numero de linha no inicio da declaracao
        if (first_non_space != std::string::npos && std::isdigit((unsigned char)expanded[first_non_space])) {
            size_t end_num = first_non_space;
            while (end_num < N && std::isdigit((unsigned char)expanded[end_num])) {
                tags[end_num++] = SyntaxTag::LINENUM;
            }
        }

        // 3. Declaracao de Label no inicio: @loop:
        if (first_non_space != std::string::npos && expanded[first_non_space] == '@') {
            size_t colon = expanded.find(':', first_non_space);
            if (colon != std::string::npos) {
                for (size_t k = first_non_space; k <= colon; ++k) {
                    tags[k] = SyntaxTag::LABEL;
                }
            }
        }

        // 4. Analisador lexico para tokens, strings, numeros e caracteres graficos
        bool in_str = false;
        size_t i = 0;
        while (i < N && (tags[i] == SyntaxTag::LINENUM || tags[i] == SyntaxTag::LABEL)) {
            i++;
        }

        while (i < N) {
            char c = expanded[i];

            if (c == '"') {
                tags[i] = SyntaxTag::STRING;
                in_str = !in_str;
                i++;
                continue;
            }

            if (in_str) {
                // Escapes dentro de strings: \", \::, \:', %x
                if (c == '\\' && i + 1 < N && expanded[i + 1] == '"') {
                    tags[i] = tags[i + 1] = SyntaxTag::ESCAPE_GRAPHIC;
                    i += 2;
                    continue;
                }
                if (c == '\\' && i + 2 < N) {
                    tags[i] = tags[i + 1] = tags[i + 2] = SyntaxTag::ESCAPE_GRAPHIC;
                    i += 3;
                    continue;
                }
                if (c == '%' && i + 1 < N && (std::isalnum((unsigned char)expanded[i + 1]) || expanded[i + 1] == ' ')) {
                    tags[i] = tags[i + 1] = SyntaxTag::ESCAPE_GRAPHIC;
                    i += 2;
                    continue;
                }
                tags[i] = SyntaxTag::STRING;
                i++;
                continue;
            }

            // Fora de strings:
            // Verificacao de REM
            if (i + 3 <= N) {
                std::string sub3 = expanded.substr(i, 3);
                std::transform(sub3.begin(), sub3.end(), sub3.begin(), ::tolower);
                if (sub3 == "rem") {
                    bool l_ok = (i == 0 || !std::isalpha((unsigned char)expanded[i - 1]));
                    bool r_ok = (i + 3 == N || !std::isalpha((unsigned char)expanded[i + 3]));
                    if (l_ok && r_ok) {
                        tags[i] = tags[i + 1] = tags[i + 2] = SyntaxTag::KEYWORD;
                        for (size_t k = i + 3; k < N; ++k) {
                            tags[k] = SyntaxTag::COMMENT;
                        }
                        break;
                    }
                }
            }

            // Referencia a Label: @loop
            if (c == '@') {
                size_t start_lbl = i;
                i++;
                while (i < N && (std::isalnum((unsigned char)expanded[i]) || expanded[i] == '_')) {
                    i++;
                }
                for (size_t k = start_lbl; k < i; ++k) tags[k] = SyntaxTag::LABEL;
                continue;
            }

            // Caracteres Graficos / Escapes ZX-81 fora de strings
            if (c == '\\' && i + 2 < N) {
                tags[i] = tags[i + 1] = tags[i + 2] = SyntaxTag::ESCAPE_GRAPHIC;
                i += 3;
                continue;
            }
            if (c == '%' && i + 1 < N && (std::isalnum((unsigned char)expanded[i + 1]) || expanded[i + 1] == ' ')) {
                tags[i] = tags[i + 1] = SyntaxTag::ESCAPE_GRAPHIC;
                i += 2;
                continue;
            }

            // Numeros
            if (std::isdigit((unsigned char)c)) {
                tags[i] = SyntaxTag::NUMBER;
                i++;
                continue;
            }

            // Operadores
            if (c == '<' || c == '>' || c == '=' || c == '+' || c == '-' || c == '*' || c == '/') {
                if (i + 1 < N) {
                    std::string op2 = expanded.substr(i, 2);
                    if (op2 == "<>" || op2 == "<=" || op2 == ">=" || op2 == "**") {
                        tags[i] = tags[i + 1] = SyntaxTag::OPERATOR;
                        i += 2;
                        continue;
                    }
                }
                tags[i] = SyntaxTag::OPERATOR;
                i++;
                continue;
            }

            // Palavras / Tokens do ZX-81
            if (std::isalpha((unsigned char)c)) {
                size_t start_w = i;
                while (i < N && (std::isalnum((unsigned char)expanded[i]) || expanded[i] == '$')) {
                    i++;
                }
                std::string word = expanded.substr(start_w, i - start_w);

                // Tratamento especial para "GO TO" e "GO SUB"
                std::string word_lcase = word;
                std::transform(word_lcase.begin(), word_lcase.end(), word_lcase.begin(), ::tolower);
                if (word_lcase == "go") {
                    size_t next_p = expanded.find_first_not_of(" \t", i);
                    if (next_p != std::string::npos && next_p + 2 <= N) {
                        std::string sub_to = expanded.substr(next_p, 2);
                        std::transform(sub_to.begin(), sub_to.end(), sub_to.begin(), ::tolower);
                        if (sub_to == "to") {
                            for (size_t k = start_w; k < next_p + 2; ++k) tags[k] = SyntaxTag::KEYWORD;
                            i = next_p + 2;
                            continue;
                        }
                    }
                    if (next_p != std::string::npos && next_p + 3 <= N) {
                        std::string sub_sub = expanded.substr(next_p, 3);
                        std::transform(sub_sub.begin(), sub_sub.end(), sub_sub.begin(), ::tolower);
                        if (sub_sub == "sub") {
                            for (size_t k = start_w; k < next_p + 3; ++k) tags[k] = SyntaxTag::KEYWORD;
                            i = next_p + 3;
                            continue;
                        }
                    }
                }

                if (compiler::ZXText2P::is_keyword(word)) {
                    for (size_t k = start_w; k < i; ++k) {
                        tags[k] = SyntaxTag::KEYWORD;
                    }
                }
                continue;
            }

            tags[i] = SyntaxTag::NORMAL;
            i++;
        }
    }

    // 5. Renderizacao estrita em inner_w colunas visiveis
    std::ostringstream out;
    SyntaxTag cur_tag = SyntaxTag::NORMAL;
    bool was_sel = false;
    out << theme.text_fg << theme.text_bg;

    for (int col = 0; col < inner_w; ++col) {
        int doc_idx = scroll_col + col;
        bool is_sel = (sel_c_start >= 0 && doc_idx >= sel_c_start && doc_idx < sel_c_end);

        if (is_sel) {
            if (!was_sel) {
                out << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg;
                was_sel = true;
            }
        } else {
            if (was_sel) {
                out << get_syntax_color(cur_tag, theme);
                was_sel = false;
            }
        }

        if (doc_idx < (int)N) {
            if (!is_sel) {
                SyntaxTag t = tags[doc_idx];
                if (t != cur_tag) {
                    cur_tag = t;
                    out << get_syntax_color(cur_tag, theme);
                }
            }
            out << expanded[doc_idx];
        } else {
            if (!is_sel) {
                if (cur_tag != SyntaxTag::NORMAL) {
                    cur_tag = SyntaxTag::NORMAL;
                    out << theme.text_fg << theme.text_bg;
                }
            }
            out << ' ';
        }
    }
    out << theme.text_fg << theme.text_bg;
    return out.str();
}

} // anonymous namespace

int run_text_editor(const std::string& file_path, EditorPresentation presentation, EditorStyle style) {
    (void)style;

    // 1. Carregar arquivo ou iniciar novo buffer vazio
    std::vector<std::string> lines;
    bool is_new_file = true;
    {
        std::ifstream file(file_path, std::ios::in);
        if (file.is_open()) {
            is_new_file = false;
            std::string line;
            while (std::getline(file, line)) {
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                lines.push_back(expand_tabs(line));
            }
            file.close();
        }
    }
    if (lines.empty()) {
        lines.push_back("");
    }

    // 2. Configuracao inicial do console
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

    DWORD prev_out_mode = 0;
    DWORD prev_in_mode = 0;
    CONSOLE_CURSOR_INFO orig_cci;
    bool has_orig_cci = false;

    if (hOut != INVALID_HANDLE_VALUE) {
        GetConsoleMode(hOut, &prev_out_mode);
        SetConsoleMode(hOut, prev_out_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

        if (GetConsoleCursorInfo(hOut, &orig_cci)) {
            has_orig_cci = true;
            CONSOLE_CURSOR_INFO block_cci = orig_cci;
            block_cci.dwSize = 100; // 100% de preenchimento = bloco quadrado solido MS-DOS!
            block_cci.bVisible = TRUE;
            SetConsoleCursorInfo(hOut, &block_cci);
        }
    }
    if (hIn != INVALID_HANDLE_VALUE) {
        GetConsoleMode(hIn, &prev_in_mode);
        SetConsoleMode(hIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
    }

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::cout << "\x1b[1 q"; // Sequencia ANSI VT para cursor de bloco quadrado piscando (MS-DOS)

    std::string current_file_path = file_path;
    if (current_file_path.empty()) {
        current_file_path = "NONAME.BAS";
    }

    std::string display_name = extract_file_name(current_file_path);
    if (display_name.empty()) {
        display_name = "NONAME.BAS";
    }

    // Menus Superiores (Linha 0)
    auto menus = build_editor_menus(presentation);
    bool menu_active = false;
    int active_menu_idx = 0;
    int active_item_idx = 0;
    bool show_about_modal = false;

    // Estado do Editor
    int cursor_row = 0;
    int cursor_col = 0;
    int scroll_row = 0;
    int scroll_col = 0;
    bool modified = false;
    bool running = true;
    bool maximized = false;
    int exit_return_code = 0; // 0 = fechar janela (continua no desktop TUI), 1 = sair da aplicacao (Alt+X)
    bool exit_dlg_is_close_window = false;
    std::string status_msg = is_new_file ? "[ Novo Arquivo ]" : "[ Arquivo Carregado ]";
    auto status_msg_time = std::chrono::steady_clock::now();

    // Estado de Selecao de Texto
    bool has_selection = false;
    int sel_start_row = 0;
    int sel_start_col = 0;
    int sel_end_row = 0;
    int sel_end_col = 0;
    bool is_mouse_selecting = false;

    // Dimensoes da janela
    int term_w = 80;
    int term_h = 25;
    get_terminal_size(term_w, term_h);

    int win_x = 0;
    int win_y = 1;
    int win_w = 68;
    int win_h = 17;

    if (presentation == EditorPresentation::TUI_INTEGRATED) {
        // Janela flutuante centralizada sobre o desktop da TUI
        // Deixa margens visíveis em todas as 4 direções para exibir o quadriculado do desktop
        win_w = std::min(70, std::max(42, term_w - 12));
        win_h = std::min(17, std::max(10, term_h - 7));
        win_x = (term_w - win_w) / 2;
        win_y = 1 + std::max(0, ((term_h - 2) - win_h) / 2);
    } else {
        // Modo CLI Standalone: janela ampla centralizada
        win_w = std::min(76, std::max(48, term_w - 4));
        win_h = std::min(21, std::max(12, term_h - 4));
        win_x = (term_w - win_w) / 2;
        win_y = 1 + std::max(0, ((term_h - 2) - win_h) / 2);
    }

    int restored_x = win_x;
    int restored_y = win_y;
    int restored_w = win_w;
    int restored_h = win_h;

    bool is_dragging = false;
    int drag_offset_x = 0;
    int drag_offset_y = 0;
    bool is_resizing = false;
    DWORD prev_btn_state = 0;

    // Modais e Dialogos
    bool show_exit_dlg = false;
    int exit_dlg_choice = 0; // 0 = Sim, 1 = Nao, 2 = Cancelar

    bool show_graphics_modal = false;
    int graphics_sel_idx = 7; // Comeca destacando \:' (bloco classico)

    bool show_compile_error_modal = false;
    std::string compile_error_msg = "";
    int compile_error_line = 0;

    bool show_compile_success_modal = false;
    std::string compile_success_msg = "";

    bool show_help_modal = false;

    bool show_compiler_opts_modal = false;
    int compiler_opts_focus = 0; // 0: labels_mode, 1: start_line, 2: line_inc

    // Obter tema atual
    auto& cfg = core::ConfigManager::instance();
    std::string theme_name = cfg.get_theme();
    EditorTheme theme = get_editor_theme(theme_name);

    std::cout << Colors::HIDE_CURSOR;

    // Funcao centralizada para execucao de acoes do menu / atalhos
    auto execute_action = [&](int action_id) {
        if (action_id == 101) { // Novo
            if (modified) {
                show_exit_dlg = true;
                exit_dlg_choice = 0;
                exit_dlg_is_close_window = false;
            } else {
                lines.clear();
                lines.push_back("");
                cursor_row = 0;
                cursor_col = 0;
                scroll_row = 0;
                scroll_col = 0;
                modified = false;
                current_file_path = "NONAME.BAS";
                display_name = "NONAME.BAS";
                status_msg = "[ Novo Arquivo: NONAME.BAS ]";
                status_msg_time = std::chrono::steady_clock::now();
            }
        } else if (action_id == 102) { // Salvar .BAS (ASCII)
            std::ofstream out(current_file_path, std::ios::out);
            for (size_t i = 0; i < lines.size(); ++i) {
                out << lines[i];
                if (i + 1 < lines.size()) out << "\n";
            }
            out.close();
            modified = false;
            status_msg = "[ Salvo com Sucesso: " + extract_file_name(current_file_path) + " ]";
            status_msg_time = std::chrono::steady_clock::now();
        } else if (action_id == 103) { // Sair da TUI / aplicacao (Alt+X)
            if (modified) {
                show_exit_dlg = true;
                exit_dlg_choice = 0;
                exit_dlg_is_close_window = false;
            } else {
                exit_return_code = 1;
                running = false;
            }
        } else if (action_id == 104) { // Fechar Janela do Editor ([×] / Ctrl+F4)
            if (modified) {
                show_exit_dlg = true;
                exit_dlg_choice = 0;
                exit_dlg_is_close_window = true;
            } else {
                exit_return_code = 0;
                running = false;
            }
        } else if (action_id == 201) { // Caracteres Graficos
            show_graphics_modal = true;
        } else if (action_id == 202) { // Inverter Video ZX-81 (%)
            bool inverted_something = false;

            if (has_selection) {
                int r1 = sel_start_row, c1 = sel_start_col;
                int r2 = sel_end_row, c2 = sel_end_col;
                if (r1 > r2 || (r1 == r2 && c1 > c2)) {
                    std::swap(r1, r2);
                    std::swap(c1, c2);
                }

                for (int r = r1; r <= r2 && r < (int)lines.size(); ++r) {
                    int line_sz = (int)lines[r].size();
                    int from_c = (r == r1) ? c1 : 0;
                    int to_c = (r == r2) ? c2 : line_sz;
                    from_c = std::max(0, std::min(line_sz, from_c));
                    to_c = std::max(from_c, std::min(line_sz, to_c));

                    if (from_c < to_c) {
                        std::string target = lines[r].substr(from_c, to_c - from_c);
                        std::string transformed = invert_zx_string(target);
                        lines[r].replace(from_c, to_c - from_c, transformed);
                        inverted_something = true;
                        if (r == r2) {
                            cursor_col = from_c + (int)transformed.size();
                        }
                    }
                }
                has_selection = false;
            } else {
                if (cursor_row >= 0 && cursor_row < (int)lines.size()) {
                    std::string& cur_line = lines[cursor_row];
                    int best_start = -1;
                    int best_end = -1;
                    bool in_q = false;
                    int q_start = -1;

                    for (int i = 0; i < (int)cur_line.size(); ++i) {
                        if (cur_line[i] == '"') {
                            if (!in_q) {
                                in_q = true;
                                q_start = i;
                            } else {
                                in_q = false;
                                int q_end = i;
                                if (cursor_col >= q_start && cursor_col <= q_end + 1) {
                                    best_start = q_start + 1;
                                    best_end = q_end;
                                    break;
                                }
                            }
                        }
                    }

                    if (best_start != -1 && best_end >= best_start) {
                        std::string target = cur_line.substr(best_start, best_end - best_start);
                        std::string transformed = invert_zx_string(target);
                        cur_line.replace(best_start, best_end - best_start, transformed);
                        inverted_something = true;
                        cursor_col = best_start + (int)transformed.size();
                    } else {
                        // Palavra sob o cursor
                        int w_start = cursor_col;
                        while (w_start > 0 && !std::isspace((unsigned char)cur_line[w_start - 1]) && cur_line[w_start - 1] != '"') {
                            w_start--;
                        }
                        int w_end = cursor_col;
                        while (w_end < (int)cur_line.size() && !std::isspace((unsigned char)cur_line[w_end]) && cur_line[w_end] != '"') {
                            w_end++;
                        }
                        if (w_end > w_start) {
                            std::string target = cur_line.substr(w_start, w_end - w_start);
                            std::string transformed = invert_zx_string(target);
                            cur_line.replace(w_start, w_end - w_start, transformed);
                            inverted_something = true;
                            cursor_col = w_start + (int)transformed.size();
                        }
                    }
                }
            }

            if (inverted_something) {
                modified = true;
                status_msg = "[ Texto invertido com sucesso (prefixo '%' ZX-81) ]";
                status_msg_time = std::chrono::steady_clock::now();
            } else {
                status_msg = "[ Posicione o cursor dentro de uma string \"...\" ou selecione o texto ]";
                status_msg_time = std::chrono::steady_clock::now();
            }
        } else if (action_id == 301) { // Compilar para .P (ZX-81)
            // 1. Auto-salva arquivo fonte .BAS primeiro
            {
                std::ofstream out(current_file_path, std::ios::out);
                for (size_t i = 0; i < lines.size(); ++i) {
                    out << lines[i];
                    if (i + 1 < lines.size()) out << "\n";
                }
                out.close();
                modified = false;
            }

            // 2. Determina nome do arquivo binario .P
            std::string p_filename = current_file_path;
            size_t dot_pos = p_filename.find_last_of('.');
            if (dot_pos != std::string::npos) {
                p_filename = p_filename.substr(0, dot_pos) + ".P";
            } else {
                p_filename += ".P";
            }

            // 3. Monta o codigo fonte completo
            std::string full_src = "";
            for (size_t i = 0; i < lines.size(); ++i) {
                full_src += lines[i] + "\n";
            }

            // 4. Executa a compilacao com ZXText2P nativo
            compiler::CompilerOptions c_opts;
            c_opts.use_labels = cfg.get_compiler_labels_mode();
            c_opts.auto_detect_mode = true;
            c_opts.start_line = cfg.get_compiler_start_line();
            c_opts.line_increment = cfg.get_compiler_line_increment();

            auto res = compiler::ZXText2P::compile_source(full_src, c_opts);
            if (res.success) {
                bool saved = compiler::ZXText2P::save_p_file(p_filename, res.p_data);
                if (saved) {
                    compile_success_msg = extract_file_name(p_filename) + " (" + 
                                          std::to_string(res.total_p_bytes) + " bytes, " + 
                                          std::to_string(res.total_lines) + " linhas)";
                    show_compile_success_modal = true;
                    status_msg = "[ Compilado com Sucesso -> " + extract_file_name(p_filename) + " ]";
                    status_msg_time = std::chrono::steady_clock::now();
                } else {
                    compile_error_msg = "Falha ao gravar arquivo no disco: " + p_filename;
                    compile_error_line = 0;
                    show_compile_error_modal = true;
                }
            } else {
                compile_error_msg = res.error_message;
                compile_error_line = res.error_line;
                show_compile_error_modal = true;
                status_msg = "[ Erro na Linha " + std::to_string(res.error_line) + ": " + res.error_message + " ]";
                status_msg_time = std::chrono::steady_clock::now();
            }
        } else if (action_id == 302) { // Opcoes do Compilador
            show_compiler_opts_modal = true;
            compiler_opts_focus = 0;
        } else if (action_id >= 401 && action_id <= 404) { // Temas
            std::string new_t = "normal";
            if (action_id == 402) new_t = "invertido";
            else if (action_id == 403) new_t = "claro";
            else if (action_id == 404) new_t = "escuro";
            cfg.set_theme(new_t);
            theme_name = new_t;
            theme = get_editor_theme(theme_name);
            status_msg = "[ Tema: " + theme_name + " ]";
            status_msg_time = std::chrono::steady_clock::now();
        } else if (action_id == 501) { // Ajuda
            show_help_modal = true;
        } else if (action_id == 502) { // Sobre
            show_about_modal = true;
        }
    };

    // Redesenho ultrarrapido apenas dos itens do menu dropdown (0.05ms, sem repaint de tela nem flicker)
    auto redraw_dropdown_fast = [&]() {
        if (!menu_active || active_menu_idx < 0 || active_menu_idx >= (int)menus.size()) return;
        const auto& cur_menu = menus[active_menu_idx];
        int drop_x = cur_menu.x_pos;
        int drop_y = 1;
        int drop_inner_w = 26;
        for (const auto& it : cur_menu.items) {
            int len = get_visible_width(it.label) + get_visible_width(it.shortcut) + 4;
            if (len > drop_inner_w) drop_inner_w = len;
        }
        int num_items = (int)cur_menu.items.size();
        std::ostringstream dbuf;
        for (int i = 0; i < num_items; ++i) {
            dbuf << "\x1b[" << (drop_y + 2 + i) << ";" << (drop_x + 2) << "H";
            std::string item_str = format_menu_item(cur_menu.items[i].label, cur_menu.items[i].shortcut, drop_inner_w);
            if (i == active_item_idx) {
                dbuf << theme.drop_sel_fg << theme.drop_sel_bg << item_str;
            } else {
                dbuf << theme.drop_item_fg << theme.drop_item_bg << item_str;
            }
        }
        dbuf << Colors::RESET << Colors::HIDE_CURSOR;
        std::cout << dbuf.str() << std::flush;
    };

    int inner_w = std::max(1, win_w - 2);
    int inner_h = std::max(1, win_h - 2);
    int total_lines = (int)lines.size();
    int v_track_len = std::max(1, inner_h - 2);
    int h_track_len = std::max(1, inner_w - 8);
    int max_col_observed = 80;
    bool needs_render = true;
    const auto& gfx_table = compiler::ZXText2P::get_graphics_table();

    // Loop Principal de Edicao
    while (running) {
        if (needs_render) {
            get_terminal_size(term_w, term_h);

            if (maximized) {
                win_x = 0;
                win_y = 1;
                win_w = term_w;
                win_h = std::max(3, term_h - 2);
            } else {
                if (win_w > term_w) win_w = term_w;
                if (win_h > term_h - 2) win_h = term_h - 2;
                if (win_x + win_w > term_w) win_x = std::max(0, term_w - win_w);
                if (win_y < 1) win_y = 1;
                if (win_y + win_h > term_h - 1) win_y = std::max(1, term_h - 1 - win_h);
            }

            inner_w = std::max(1, win_w - 2);
            inner_h = std::max(1, win_h - 2);

            cursor_row = std::max(0, std::min((int)lines.size() - 1, cursor_row));
            int cur_line_len = (int)lines[cursor_row].size();
            cursor_col = std::max(0, std::min(cur_line_len, cursor_col));

            if (cursor_row < scroll_row) {
                scroll_row = cursor_row;
            } else if (cursor_row >= scroll_row + inner_h) {
                scroll_row = cursor_row - inner_h + 1;
            }

            if (cursor_col < scroll_col) {
                scroll_col = cursor_col;
            } else if (cursor_col >= scroll_col + inner_w) {
                scroll_col = cursor_col - inner_w + 1;
            }

        // =============================================================
        // RENDERIZACAO EM BUFFER STRING
        // =============================================================
        std::ostringstream buf;
        buf << Colors::HOME << Colors::HIDE_CURSOR;

            total_lines = (int)lines.size();
            v_track_len = std::max(1, inner_h - 2);
            int v_thumb_pos = 0;
            if (total_lines > 1) {
                float v_ratio = (float)cursor_row / (float)(total_lines - 1);
                v_thumb_pos = (int)(v_ratio * (v_track_len - 1));
            }

            max_col_observed = 80;
            for (const auto& l : lines) {
                if ((int)l.size() > max_col_observed) max_col_observed = (int)l.size();
            }
            h_track_len = std::max(1, inner_w - 8);
            int h_thumb_pos = 0;
            if (max_col_observed > 0) {
                float h_ratio = (float)cursor_col / (float)max_col_observed;
                h_thumb_pos = (int)(h_ratio * (h_track_len - 1));
            }

        // -------------------------------------------------------------
        // LINHA 0: BARRA DE MENUS SUPERIOR (MS-DOS / Turbo C++)
        // -------------------------------------------------------------
        buf << theme.menu_bar_fg << theme.menu_bar_bg;
        int m_col = 0;
        for (size_t m = 0; m < menus.size(); ++m) {
            bool is_active_menu = (menu_active && (int)m == active_menu_idx);
            std::string m_title = " " + menus[m].title + " ";
            if (is_active_menu) {
                buf << theme.menu_active_fg << theme.menu_active_bg << m_title;
            } else {
                buf << theme.menu_bar_fg << theme.menu_bar_bg << m_title;
            }
            m_col += (int)m_title.size();
        }
        buf << theme.menu_bar_fg << theme.menu_bar_bg;
        while (m_col < term_w) {
            buf << ' ';
            m_col++;
        }
        buf << Colors::RESET << "\n";

        // -------------------------------------------------------------
        // LINHAS 1 A term_h - 2: DESKTOP E JANELA DO EDITOR
        // -------------------------------------------------------------
        for (int y = 1; y < term_h - 1; ++y) {
            if (y < win_y) {
                buf << theme.desktop_fg << theme.desktop_bg;
                for (int x = 0; x < term_w; ++x) {
                    buf << theme.desktop_char;
                }
                buf << Colors::RESET << "\n";
                continue;
            }

            if (y >= win_y && y < win_y + win_h) {
                // 1. Desktop a esquerda da janela
                if (win_x > 0) {
                    buf << theme.desktop_fg << theme.desktop_bg;
                    for (int x = 0; x < win_x; ++x) {
                        buf << theme.desktop_char;
                    }
                }

                // 2. Conteudo da janela (win_w colunas)
                if (y == win_y) {
                    // Barra de Titulo
                    buf << theme.frame_fg << theme.frame_bg;
                    buf << "╔";

                    // Botao fechar [×]
                    buf << "═[";
                    buf << theme.btn_fg << theme.btn_bg << "×";
                    buf << theme.frame_fg << theme.frame_bg << "]═";

                    // Titulo com o nome do arquivo
                    std::string title_str = " " + display_name + (modified ? " *" : "") + " ";
                    int available_title_space = win_w - 12;
                    if ((int)title_str.size() > available_title_space && available_title_space > 0) {
                        title_str = title_str.substr(0, available_title_space);
                    }

                    int remaining_bar = win_w - 12 - (int)title_str.size();
                    if (remaining_bar < 0) remaining_bar = 0;
                    int left_bar = remaining_bar / 2;
                    int right_bar = remaining_bar - left_bar;

                    for (int i = 0; i < left_bar; ++i) buf << "═";
                    buf << theme.title_fg << theme.title_bg << title_str;
                    buf << theme.frame_fg << theme.frame_bg;
                    for (int i = 0; i < right_bar; ++i) buf << "═";

                    // Botao maximizar/restaurar
                    buf << "═[";
                    buf << theme.btn_fg << theme.btn_bg;
                    if (maximized) {
                        buf << "↕";
                    } else {
                        buf << "▲";
                    }
                    buf << theme.frame_fg << theme.frame_bg << "]═";

                    buf << "╗";
                } else if (y > win_y && y < win_y + win_h - 1) {
                    int vr = y - win_y - 1;
                    int doc_line_idx = scroll_row + vr;

                    buf << theme.frame_fg << theme.frame_bg << "║";

                    // Renderizacao da linha com Destaque de Sintaxe e Selecao
                    if (doc_line_idx < (int)lines.size()) {
                        int sel_c_start = -1;
                        int sel_c_end = -1;
                        if (has_selection) {
                            int r1 = sel_start_row, c1 = sel_start_col;
                            int r2 = sel_end_row, c2 = sel_end_col;
                            if (r1 > r2 || (r1 == r2 && c1 > c2)) {
                                std::swap(r1, r2);
                                std::swap(c1, c2);
                            }
                            if (doc_line_idx >= r1 && doc_line_idx <= r2) {
                                sel_c_start = (doc_line_idx == r1) ? c1 : 0;
                                sel_c_end = (doc_line_idx == r2) ? c2 : (int)lines[doc_line_idx].size();
                            }
                        }
                        buf << render_editor_line(lines[doc_line_idx], scroll_col, inner_w, theme, sel_c_start, sel_c_end);
                    } else {
                        buf << theme.text_fg << theme.text_bg << std::string(inner_w, ' ');
                    }

                    // Barra de rolagem vertical
                    if (vr == 0) {
                        buf << theme.scroll_arrow_fg << theme.scroll_arrow_bg << "▲";
                    } else if (vr == inner_h - 1) {
                        buf << theme.scroll_arrow_fg << theme.scroll_arrow_bg << "▼";
                    } else {
                        int track_y = vr - 1;
                        if (track_y == v_thumb_pos) {
                            buf << theme.scroll_thumb_fg << theme.scroll_thumb_bg << "█";
                        } else {
                            buf << theme.scroll_track_fg << theme.scroll_track_bg << "░";
                        }
                    }
                } else if (y == win_y + win_h - 1) {
                    buf << theme.frame_fg << theme.frame_bg << "╚";

                    buf << theme.scroll_arrow_fg << theme.scroll_arrow_bg << "◄";

                    for (int i = 0; i < h_track_len; ++i) {
                        if (i == h_thumb_pos) {
                            buf << theme.scroll_thumb_fg << theme.scroll_thumb_bg << "█";
                        } else {
                            buf << theme.scroll_track_fg << theme.scroll_track_bg << "░";
                        }
                    }

                    buf << theme.scroll_arrow_fg << theme.scroll_arrow_bg << "►";

                    int remaining_bottom = win_w - 4 - h_track_len;
                    std::string pos_tag = " " + std::to_string(cursor_row + 1) + ":" + std::to_string(cursor_col + 1) + " ";
                    if ((int)pos_tag.size() <= remaining_bottom) {
                        int pad = remaining_bottom - (int)pos_tag.size();
                        for (int i = 0; i < pad; ++i) buf << theme.frame_fg << theme.frame_bg << "═";
                        buf << theme.frame_fg << theme.frame_bg << pos_tag;
                    } else {
                        for (int i = 0; i < remaining_bottom; ++i) buf << theme.frame_fg << theme.frame_bg << "═";
                    }

                    buf << theme.btn_fg << theme.btn_bg << "◢";
                }

                // 3. Desktop a direita da janela (com sombra lateral se nao maximizado)
                int right_space = term_w - (win_x + win_w);
                if (right_space > 0) {
                    int shadow_cols = (!maximized && y > win_y) ? std::min(2, right_space) : 0;
                    if (shadow_cols > 0) {
                        buf << theme.drop_shadow_fg << theme.drop_shadow_bg;
                        for (int s = 0; s < shadow_cols; ++s) {
                            buf << "▒";
                        }
                    }
                    int rem_desk = right_space - shadow_cols;
                    if (rem_desk > 0) {
                        buf << theme.desktop_fg << theme.desktop_bg;
                        for (int x = 0; x < rem_desk; ++x) {
                            buf << theme.desktop_char;
                        }
                    }
                }

                buf << Colors::RESET << "\n";
                continue;
            }

            // Linha da sombra inferior da janela (y == win_y + win_h)
            if (!maximized && y == win_y + win_h) {
                int start_shadow = std::min(term_w, win_x + 2);
                int end_shadow = std::min(term_w, win_x + win_w + 2);

                if (start_shadow > 0) {
                    buf << theme.desktop_fg << theme.desktop_bg;
                    for (int x = 0; x < start_shadow; ++x) {
                        buf << theme.desktop_char;
                    }
                }
                if (end_shadow > start_shadow) {
                    buf << theme.drop_shadow_fg << theme.drop_shadow_bg;
                    for (int x = start_shadow; x < end_shadow; ++x) {
                        buf << "▒";
                    }
                }
                if (term_w > end_shadow) {
                    buf << theme.desktop_fg << theme.desktop_bg;
                    for (int x = end_shadow; x < term_w; ++x) {
                        buf << theme.desktop_char;
                    }
                }
                buf << Colors::RESET << "\n";
                continue;
            }

            // Linhas restantes abaixo da sombra: puro desktop
            buf << theme.desktop_fg << theme.desktop_bg;
            for (int x = 0; x < term_w; ++x) {
                buf << theme.desktop_char;
            }
            buf << Colors::RESET << "\n";
        }

        // -------------------------------------------------------------
        // LINHA INFERIOR DO TERMINAL: BARRA DE ATALHOS / STATUS DO SISTEMA
        // -------------------------------------------------------------
        {
            buf << theme.status_fg << theme.status_bg;
            std::string esc_desc = (presentation == EditorPresentation::TUI_INTEGRATED) ? "Esc:Fechar" : "Esc:Sair";
            std::string help_bar = " F1:Ajuda  F2:Salvar  F4:Graficos  F5:Zoom  F7:Config  F9:Compilar.P  F10:Menu  " + esc_desc + " | L:" + 
                                   std::to_string(cursor_row + 1) + " C:" + std::to_string(cursor_col + 1);

            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - status_msg_time).count() < 3) {
                help_bar = " " + status_msg + " | " + help_bar;
            }

            if ((int)help_bar.size() > term_w) {
                help_bar = help_bar.substr(0, term_w);
            }
            buf << help_bar;
            int rem = term_w - (int)help_bar.size();
            if (rem > 0) {
                buf << std::string(rem, ' ');
            }
            buf << Colors::RESET;
        }

        // =============================================================
        // RENDERIZACAO DE DROPDOWNS E MODAIS
        // =============================================================

        // DROPDOWN MENU ABERTO (se menu_active == true)
        if (menu_active && active_menu_idx >= 0 && active_menu_idx < (int)menus.size()) {
            const auto& cur_menu = menus[active_menu_idx];
            int drop_x = cur_menu.x_pos;
            int drop_y = 1; // Linha logo abaixo da barra de menus
            int drop_inner_w = 26;
            for (const auto& it : cur_menu.items) {
                int len = get_visible_width(it.label) + get_visible_width(it.shortcut) + 4;
                if (len > drop_inner_w) drop_inner_w = len;
            }
            int drop_w = drop_inner_w + 2;
            int num_items = (int)cur_menu.items.size();

            // Borda superior
            buf << "\x1b[" << (drop_y + 1) << ";" << (drop_x + 1) << "H";
            buf << theme.drop_border_fg << theme.drop_border_bg << "┌" << repeat_utf8("─", drop_inner_w) << "┐";

            // Itens do dropdown
            for (int i = 0; i < num_items; ++i) {
                buf << "\x1b[" << (drop_y + 2 + i) << ";" << (drop_x + 1) << "H";
                buf << theme.drop_border_fg << theme.drop_border_bg << "│";

                std::string item_str = format_menu_item(cur_menu.items[i].label, cur_menu.items[i].shortcut, drop_inner_w);
                if (i == active_item_idx) {
                    buf << theme.drop_sel_fg << theme.drop_sel_bg << item_str;
                } else {
                    buf << theme.drop_item_fg << theme.drop_item_bg << item_str;
                }
                buf << theme.drop_border_fg << theme.drop_border_bg << "│";

                // Sombra lateral direita MS-DOS
                buf << theme.drop_shadow_fg << theme.drop_shadow_bg << "▒";
            }

            // Borda inferior
            buf << "\x1b[" << (drop_y + 2 + num_items) << ";" << (drop_x + 1) << "H";
            buf << theme.drop_border_fg << theme.drop_border_bg << "└" << repeat_utf8("─", drop_inner_w) << "┘";
            buf << theme.drop_shadow_fg << theme.drop_shadow_bg << "▒";

            // Sombra inferior MS-DOS
            buf << "\x1b[" << (drop_y + 3 + num_items) << ";" << (drop_x + 2) << "H";
            buf << theme.drop_shadow_fg << theme.drop_shadow_bg << repeat_utf8("▒", drop_w);
            buf << Colors::RESET;
        }

        // 1. MODAL DE CONFIRMACAO DE SAIDA
        if (show_exit_dlg) {
            int dlg_w = 48;
            int dlg_h = 7;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            std::string dlg_title = exit_dlg_is_close_window ? "[ Fechar Janela ]" : "[ Salvar e Sair ]";
            render_dialog_top(buf, dlg_x, dlg_y, dlg_w, dlg_title, theme);

            std::string header_txt = exit_dlg_is_close_window ? "        Fechar Janela do Editor" 
                                                              : "               Salvar e Sair";
            render_dialog_row(buf, dlg_x, dlg_y + 1, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + header_txt, theme);
            render_dialog_row(buf, dlg_x, dlg_y + 2, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  O arquivo foi modificado. Salvar agora?", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 3, dlg_w, "", theme);

            std::string btn_line = "  ";
            if (exit_dlg_choice == 0) btn_line += theme.dlg_btn_sel_fg + theme.dlg_btn_sel_bg + "[ Sim (S) ]" + theme.dlg_body_bg;
            else btn_line += theme.dlg_btn_fg + theme.dlg_btn_bg + "[ Sim (S) ]" + theme.dlg_body_bg;
            btn_line += "  ";
            if (exit_dlg_choice == 1) btn_line += theme.dlg_btn_sel_fg + theme.dlg_btn_sel_bg + "[ Nao (N) ]" + theme.dlg_body_bg;
            else btn_line += theme.dlg_btn_fg + theme.dlg_btn_bg + "[ Nao (N) ]" + theme.dlg_body_bg;
            btn_line += "  ";
            if (exit_dlg_choice == 2) btn_line += theme.dlg_btn_sel_fg + theme.dlg_btn_sel_bg + "[ Cancelar (C) ]" + theme.dlg_body_bg;
            else btn_line += theme.dlg_btn_fg + theme.dlg_btn_bg + "[ Cancelar (C) ]" + theme.dlg_body_bg;

            render_dialog_row(buf, dlg_x, dlg_y + 4, dlg_w, btn_line, theme);
            render_dialog_row(buf, dlg_x, dlg_y + 5, dlg_w, "", theme);
            render_dialog_bottom(buf, dlg_x, dlg_y + 6, dlg_w, theme);
        }

        // 2. MODAL SELETOR DE CARACTERES GRAFICOS (F4 / Ctrl+G)
        if (show_graphics_modal) {
            int dlg_w = 64;
            int dlg_h = 18;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            render_dialog_top(buf, dlg_x, dlg_y, dlg_w, "[ Caracteres Graficos do ZX-81 (F4) ]", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 1, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + " Navegue com as setas e tecle Enter para inserir o escape:", theme);

            for (int row = 0; row < 12; ++row) {
                int idx1 = row;
                int idx2 = row + 12;

                std::string row_str = " ";

                // Coluna 1
                if (idx1 < (int)gfx_table.size()) {
                    bool sel1 = (graphics_sel_idx == idx1);
                    std::string label1 = " [" + gfx_table[idx1].glyph_utf8 + "] " + gfx_table[idx1].escape_seq + " ";
                    int v_w = get_visible_width(label1);
                    if (v_w < 12) label1.append(12 - v_w, ' ');

                    if (sel1) row_str += theme.dlg_btn_sel_fg + theme.dlg_btn_sel_bg + label1 + theme.dlg_body_bg;
                    else row_str += theme.dlg_text_fg + theme.dlg_body_bg + label1;
                } else {
                    row_str += std::string(12, ' ');
                }

                row_str += "   │  ";

                // Coluna 2
                if (idx2 < (int)gfx_table.size()) {
                    bool sel2 = (graphics_sel_idx == idx2);
                    std::string label2 = " [" + gfx_table[idx2].glyph_utf8 + "] " + gfx_table[idx2].escape_seq + " ";
                    int v_w = get_visible_width(label2);
                    if (v_w < 12) label2.append(12 - v_w, ' ');

                    if (sel2) row_str += theme.dlg_btn_sel_fg + theme.dlg_btn_sel_bg + label2 + theme.dlg_body_bg;
                    else row_str += theme.dlg_text_fg + theme.dlg_body_bg + label2;
                } else {
                    row_str += std::string(12, ' ');
                }

                render_dialog_row(buf, dlg_x, dlg_y + 2 + row, dlg_w, row_str, theme);
            }

            // Linha com descricao do selecionado
            std::string desc_line = " " + gfx_table[graphics_sel_idx].description + 
                                    " (ZX: " + std::to_string(gfx_table[graphics_sel_idx].zx_code) + ") ";
            int max_desc_w = dlg_w - 4;
            if (get_visible_width(desc_line) > max_desc_w) {
                desc_line = desc_line.substr(0, max_desc_w);
            }
            render_dialog_row(buf, dlg_x, dlg_y + 14, dlg_w, theme.dlg_btn_sel_fg + theme.dlg_btn_sel_bg + desc_line + theme.dlg_body_bg, theme);

            render_dialog_row(buf, dlg_x, dlg_y + 15, dlg_w, theme.dlg_body_bg + "    [ Enter: Inserir no Editor ]        [ Esc: Cancelar ]", theme);
            render_dialog_bottom(buf, dlg_x, dlg_y + 16, dlg_w, theme);
        }

        // 3. MODAL DE ERRO DE COMPILACAO
        if (show_compile_error_modal) {
            int dlg_w = 60;
            int dlg_h = 7;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            render_dialog_top(buf, dlg_x, dlg_y, dlg_w, "[ Erro de Compilacao ZXText2P ]", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 1, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  Falha ao gerar binario .P do Sinclair ZX-81:", theme);

            std::string err_line_str = "  Linha " + std::to_string(compile_error_line) + ": " + compile_error_msg;
            int max_err_w = dlg_w - 4;
            if (get_visible_width(err_line_str) > max_err_w) {
                err_line_str = err_line_str.substr(0, max_err_w);
            }
            render_dialog_row(buf, dlg_x, dlg_y + 2, dlg_w, theme.dlg_btn_sel_fg + theme.dlg_btn_sel_bg + err_line_str + theme.dlg_body_bg, theme);
            render_dialog_row(buf, dlg_x, dlg_y + 3, dlg_w, "", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 4, dlg_w, theme.dlg_body_bg + "      [ Enter / Esc: Ir ate a Linha do Erro ]", theme);
            render_dialog_bottom(buf, dlg_x, dlg_y + 5, dlg_w, theme);
        }

        // 4. MODAL DE SUCESSO DE COMPILACAO
        if (show_compile_success_modal) {
            int dlg_w = 58;
            int dlg_h = 6;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            render_dialog_top(buf, dlg_x, dlg_y, dlg_w, "[ Compilacao Concluida (.P) ]", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 1, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  Programa compilado e salvo com sucesso em formato .P!", theme);

            std::string succ_line = "  " + compile_success_msg;
            int max_succ_w = dlg_w - 4;
            if (get_visible_width(succ_line) > max_succ_w) {
                succ_line = succ_line.substr(0, max_succ_w);
            }
            render_dialog_row(buf, dlg_x, dlg_y + 2, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + succ_line, theme);
            render_dialog_row(buf, dlg_x, dlg_y + 3, dlg_w, theme.dlg_body_bg + "              [ Enter / Espaco / Esc: OK ]", theme);
            render_dialog_bottom(buf, dlg_x, dlg_y + 4, dlg_w, theme);
        }

        // 5. MODAL DE AJUDA DO EDITOR (F1)
        if (show_help_modal) {
            int dlg_w = 66;
            int dlg_h = 17;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            render_dialog_top(buf, dlg_x, dlg_y, dlg_w, "[ Ajuda do Editor ZX-81 (F1) ]", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 1, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  Atalhos Principais do Editor de Texto:", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 2, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "    F1          : Exibe esta tela de ajuda", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 3, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "    F2 / Ctrl+S : Salva o arquivo ASCII (.BAS) no disco", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 4, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "    Ctrl+I      : Inverte texto selecionado / string (Video Inverso %)", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 5, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "    F4 / Ctrl+G : Seletor de Caracteres Graficos do ZX-81", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 6, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "    F5          : Maximiza / Restaura tamanho da janela", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 7, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "    F7 / Ctrl+T : Configuracoes do Compilador ZXText2P", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 8, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "    F9 / Ctrl+B : Compila o fonte para binario ZX-81 (.P)", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 9, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "    F10 / Alt+M : Ativa e foca a Barra de Menus superior", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 10, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "    Esc / Alt+X : Encerra e fecha a janela do editor", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 11, dlg_w, "", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 12, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  Recursos: Destaque de sintaxe, suporte completo a mouse", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 13, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  e menus padrao Turbo C++ / MS-DOS.", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 14, dlg_w, "", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 15, dlg_w, theme.dlg_body_bg + "               [ Pressione Enter ou Esc para Fechar ]", theme);
            render_dialog_bottom(buf, dlg_x, dlg_y + 16, dlg_w, theme);
        }

        // 6. MODAL DE OPCOES DO COMPILADOR (F7 / Ctrl+T)
        if (show_compiler_opts_modal) {
            int dlg_w = 56;
            int dlg_h = 9;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            bool labels_mode = cfg.get_compiler_labels_mode();
            int start_line = cfg.get_compiler_start_line();
            int line_inc = cfg.get_compiler_line_increment();

            render_dialog_top(buf, dlg_x, dlg_y, dlg_w, "[ Parametros do Compilador ZXText2P ]", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 1, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  Altere com Setas / Espaco e tecle Enter para gravar:", theme);

            // Opcao 0: Modo Labels
            std::string row0 = theme.dlg_body_bg + "    Modo Labels (@label): ";
            std::string lbl_val = labels_mode ? "[ Ativado (Sim) ]" : "[ Auto-Deteccao ]";
            if (compiler_opts_focus == 0) row0 += theme.dlg_btn_sel_fg + theme.dlg_btn_sel_bg + lbl_val + theme.dlg_body_bg;
            else row0 += theme.dlg_btn_fg + theme.dlg_btn_bg + lbl_val + theme.dlg_body_bg;
            render_dialog_row(buf, dlg_x, dlg_y + 2, dlg_w, row0, theme);

            // Opcao 1: Linha Inicial
            std::string row1 = theme.dlg_body_bg + "    Linha Inicial (-s)  : ";
            std::string s_val = "< " + std::to_string(start_line) + " >";
            if (compiler_opts_focus == 1) row1 += theme.dlg_btn_sel_fg + theme.dlg_btn_sel_bg + s_val + theme.dlg_body_bg;
            else row1 += theme.dlg_btn_fg + theme.dlg_btn_bg + s_val + theme.dlg_body_bg;
            render_dialog_row(buf, dlg_x, dlg_y + 3, dlg_w, row1, theme);

            // Opcao 2: Incremento
            std::string row2 = theme.dlg_body_bg + "    Incremento    (-i)  : ";
            std::string i_val = "< " + std::to_string(line_inc) + " >";
            if (compiler_opts_focus == 2) row2 += theme.dlg_btn_sel_fg + theme.dlg_btn_sel_bg + i_val + theme.dlg_body_bg;
            else row2 += theme.dlg_btn_fg + theme.dlg_btn_bg + i_val + theme.dlg_body_bg;
            render_dialog_row(buf, dlg_x, dlg_y + 4, dlg_w, row2, theme);

            render_dialog_row(buf, dlg_x, dlg_y + 5, dlg_w, "", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 6, dlg_w, theme.dlg_body_bg + "           [ Enter: Salvar ]        [ Esc: Fechar ]", theme);
            render_dialog_bottom(buf, dlg_x, dlg_y + 7, dlg_w, theme);
        }

        // 7. MODAL SOBRE (About)
        if (show_about_modal) {
            int dlg_w = 56;
            int dlg_h = 13;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            render_dialog_top(buf, dlg_x, dlg_y, dlg_w, "[ Sobre o zx81ide ]", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 1, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  zx81ide - IDE para Sinclair ZX-81 & Clones", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 2, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  Versao      : v" + get_version_string(), theme);
            render_dialog_row(buf, dlg_x, dlg_y + 3, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  Hardware (X): Sinclair ZX81 (Foundation)", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 4, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  Jogo     (Y): 3D Monster Maze", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 5, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  Compilador  : ZXText2P C++ Nativo Embutido", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 6, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  Tema Ativo  : " + theme_name + " (SQLite 3)", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 7, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  Autor       : Wilson \"Barney\" Pilon", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 8, dlg_w, theme.dlg_text_fg + theme.dlg_body_bg + "  Inspiracao  : fwMSX (Basic Dignified Suite)", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 9, dlg_w, "", theme);
            render_dialog_row(buf, dlg_x, dlg_y + 10, dlg_w, theme.dlg_btn_sel_fg + theme.dlg_btn_sel_bg + "                    [  OK (Enter/Esc)  ]" + theme.dlg_body_bg, theme);
            render_dialog_bottom(buf, dlg_x, dlg_y + 11, dlg_w, theme);
        }

        // Posicionar cursor real do console na posicao de digitacao
        bool any_modal_open = show_exit_dlg || show_graphics_modal || show_compile_error_modal || 
                              show_compile_success_modal || show_help_modal || show_compiler_opts_modal || show_about_modal || menu_active;

        if (!any_modal_open) {
            int cursor_screen_x = win_x + 1 + (cursor_col - scroll_col) + 1;
            int cursor_screen_y = win_y + 1 + (cursor_row - scroll_row) + 1;
            if (cursor_screen_x >= win_x + 2 && cursor_screen_x <= win_x + inner_w + 1 &&
                cursor_screen_y >= win_y + 2 && cursor_screen_y <= win_y + inner_h + 1) {
                buf << "\x1b[" << cursor_screen_y << ";" << cursor_screen_x << "H" << Colors::SHOW_CURSOR;
            }
        }

        std::cout << buf.str() << std::flush;
        needs_render = false;
    }

    // =============================================================
    // CAPTURA DE EVENTOS (TECLADO E MOUSE)
    // =============================================================
#ifdef _WIN32
    DWORD wait_res = WaitForSingleObject(hIn, 40);
    if (wait_res == WAIT_TIMEOUT) {
        int cur_w = 80, cur_h = 25;
        get_terminal_size(cur_w, cur_h);
        if (cur_w != term_w || cur_h != term_h) {
            term_w = cur_w;
            term_h = cur_h;
            needs_render = true;
        }
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - status_msg_time).count() == 3) {
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
            bool left_released = !(btn & FROM_LEFT_1ST_BUTTON_PRESSED) && (prev_btn_state & FROM_LEFT_1ST_BUTTON_PRESSED);
            bool left_down = (btn & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;

            // 1. Roda do Mouse (Scroll vertical)
            if (flags & MOUSE_WHEELED) {
                short delta = (short)HIWORD(rec.Event.MouseEvent.dwButtonState);
                if (delta > 0) {
                    cursor_row = std::max(0, cursor_row - 3);
                } else if (delta < 0) {
                    cursor_row = std::min((int)lines.size() - 1, cursor_row + 3);
                }
                needs_render = true;
                prev_btn_state = btn;
                continue;
            }

            // 2. Liberacao do Botao Esquerdo (Mouse Up)
            if (left_released || !left_down) {
                if (is_dragging || is_resizing) {
                    is_dragging = false;
                    is_resizing = false;
                    needs_render = true;
                }
                if (is_mouse_selecting) {
                    is_mouse_selecting = false;
                    if (sel_start_row == sel_end_row && sel_start_col == sel_end_col) {
                        has_selection = false;
                    }
                    needs_render = true;
                }
            }

            // 3. Movimentacao do Mouse (MOUSE_MOVED)
            if (flags & MOUSE_MOVED) {
                if (is_dragging && !maximized) {
                    int new_x = std::max(0, std::min(term_w - win_w, (int)mpos.X - drag_offset_x));
                    int new_y = std::max(1, std::min(term_h - 2 - win_h, (int)mpos.Y - drag_offset_y));
                    if (new_x != win_x || new_y != win_y) {
                        win_x = new_x;
                        win_y = new_y;
                        restored_x = win_x;
                        restored_y = win_y;
                        needs_render = true;
                    }
                    prev_btn_state = btn;
                    break;
                } else if (is_resizing && !maximized) {
                    int new_w = std::max(28, std::min(term_w - win_x, (int)mpos.X - win_x + 1));
                    int new_h = std::max(8, std::min(term_h - 1 - win_y, (int)mpos.Y - win_y + 1));
                    if (new_w != win_w || new_h != win_h) {
                        win_w = new_w;
                        win_h = new_h;
                        restored_w = win_w;
                        restored_h = win_h;
                        needs_render = true;
                    }
                    prev_btn_state = btn;
                    break;
                } else if (menu_active) {
                    // Navega sobre os titulos de menus se passar o mouse na linha 0
                    if (mpos.Y == 0) {
                        for (size_t m = 0; m < menus.size(); ++m) {
                            if (mpos.X >= menus[m].x_pos && mpos.X < menus[m].x_pos + menus[m].width) {
                                if (active_menu_idx != (int)m) {
                                    active_menu_idx = (int)m;
                                    active_item_idx = 0;
                                    needs_render = true;
                                }
                                break;
                            }
                        }
                    } else {
                        // Destaca item no submenu dropdown ativo sob o cursor de forma instantanea sem redesenhar a tela
                        const auto& cur_menu = menus[active_menu_idx];
                        int drop_x = cur_menu.x_pos;
                        int drop_y = 1;
                        int num_items = (int)cur_menu.items.size();
                        int drop_inner_w = 26;
                        for (const auto& it : cur_menu.items) {
                            int len = (int)it.label.size() + (int)it.shortcut.size() + 4;
                            if (len > drop_inner_w) drop_inner_w = len;
                        }
                        int drop_w = drop_inner_w + 2;

                        if (mpos.Y >= drop_y + 1 && mpos.Y <= drop_y + num_items &&
                            mpos.X >= drop_x + 1 && mpos.X <= drop_x + drop_w - 1) {
                            int new_item = mpos.Y - (drop_y + 1);
                            if (new_item != active_item_idx) {
                                active_item_idx = new_item;
                                redraw_dropdown_fast();
                            }
                        }
                    }
                } else if (is_mouse_selecting && left_down) {
                    if (mpos.Y > win_y && mpos.Y < win_y + win_h - 1 &&
                        mpos.X > win_x && mpos.X < win_x + win_w - 1) {
                        int r = scroll_row + (mpos.Y - win_y - 1);
                        int c = scroll_col + (mpos.X - win_x - 1);
                        r = std::max(0, std::min((int)lines.size() - 1, r));
                        int line_sz = (int)lines[r].size();
                        c = std::max(0, std::min(line_sz, c));

                        sel_end_row = r;
                        sel_end_col = c;
                        cursor_row = r;
                        cursor_col = c;
                        has_selection = (sel_start_row != sel_end_row || sel_start_col != sel_end_col);
                        needs_render = true;
                    }
                    prev_btn_state = btn;
                    break;
                }
            }

                // 4. Clique com Botao Esquerdo (APENAS na transicao inicial para baixo)
                if (left_clicked && !is_dragging && !is_resizing) {
                    needs_render = true;
                    // A. Se menu dropdown estiver aberto, processa clique
                    if (menu_active) {
                        if (mpos.Y == 0) {
                            bool clicked_menu = false;
                            for (size_t m = 0; m < menus.size(); ++m) {
                                if (mpos.X >= menus[m].x_pos && mpos.X < menus[m].x_pos + menus[m].width) {
                                    if (active_menu_idx == (int)m) {
                                        menu_active = false;
                                    } else {
                                        active_menu_idx = (int)m;
                                        active_item_idx = 0;
                                    }
                                    clicked_menu = true;
                                    break;
                                }
                            }
                            if (!clicked_menu) {
                                menu_active = false;
                            }
                            prev_btn_state = btn;
                            break;
                        }

                        const auto& cur_menu = menus[active_menu_idx];
                        int drop_x = cur_menu.x_pos;
                        int drop_y = 1;
                        int num_items = (int)cur_menu.items.size();
                        int drop_inner_w = 26;
                        for (const auto& it : cur_menu.items) {
                            int len = (int)it.label.size() + (int)it.shortcut.size() + 4;
                            if (len > drop_inner_w) drop_inner_w = len;
                        }
                        int drop_w = drop_inner_w + 2;

                        if (mpos.Y >= drop_y + 1 && mpos.Y <= drop_y + num_items &&
                            mpos.X >= drop_x + 1 && mpos.X <= drop_x + drop_w - 1) {
                            int clicked_idx = mpos.Y - (drop_y + 1);
                            if (clicked_idx >= 0 && clicked_idx < num_items) {
                                int act_id = cur_menu.items[clicked_idx].id;
                                menu_active = false;
                                execute_action(act_id);
                                prev_btn_state = btn;
                                break;
                            }
                        } else {
                            menu_active = false;
                            prev_btn_state = btn;
                            break;
                        }
                    }

                    // B. Modais ativos com clique
                    if (show_exit_dlg) {
                        int dlg_w = 46;
                        int dlg_h = 7;
                        int dlg_x = std::max(0, (term_w - dlg_w) / 2);
                        int dlg_y = std::max(0, (term_h - dlg_h) / 2);

                        if (mpos.Y == dlg_y + 4) {
                            if (mpos.X >= dlg_x + 3 && mpos.X <= dlg_x + 13) {
                                execute_action(102); // Salva
                                exit_return_code = (exit_dlg_is_close_window ? 0 : 1);
                                running = false;
                            } else if (mpos.X >= dlg_x + 16 && mpos.X <= dlg_x + 26) {
                                exit_return_code = (exit_dlg_is_close_window ? 0 : 1);
                                running = false;
                            } else if (mpos.X >= dlg_x + 29 && mpos.X <= dlg_x + 43) {
                                show_exit_dlg = false;
                            }
                        }
                        prev_btn_state = btn;
                        break;
                    }

                    if (show_compile_error_modal) {
                        show_compile_error_modal = false;
                        if (compile_error_line > 0 && compile_error_line <= (int)lines.size()) {
                            cursor_row = compile_error_line - 1;
                            cursor_col = 0;
                            scroll_row = std::max(0, cursor_row - inner_h / 2);
                        }
                        prev_btn_state = btn;
                        break;
                    }

                    if (show_compile_success_modal) {
                        show_compile_success_modal = false;
                        prev_btn_state = btn;
                        break;
                    }

                    if (show_about_modal) {
                        show_about_modal = false;
                        prev_btn_state = btn;
                        break;
                    }

                    if (show_help_modal) {
                        show_help_modal = false;
                        prev_btn_state = btn;
                        break;
                    }

                    if (show_compiler_opts_modal) {
                        show_compiler_opts_modal = false;
                        prev_btn_state = btn;
                        break;
                    }

                    if (show_graphics_modal) {
                        int dlg_w = 64;
                        int dlg_h = 18;
                        int dlg_x = std::max(0, (term_w - dlg_w) / 2);
                        int dlg_y = std::max(0, (term_h - dlg_h) / 2);

                        if (mpos.Y >= dlg_y + 2 && mpos.Y <= dlg_y + 13) {
                            int r = mpos.Y - (dlg_y + 2);
                            if (mpos.X >= dlg_x + 2 && mpos.X <= dlg_x + 18) {
                                graphics_sel_idx = r;
                            } else if (mpos.X >= dlg_x + 22 && mpos.X <= dlg_x + 38) {
                                graphics_sel_idx = r + 12;
                            }
                        } else if (mpos.Y == dlg_y + 15) {
                            if (mpos.X >= dlg_x + 4 && mpos.X <= dlg_x + 32) {
                                std::string esc = gfx_table[graphics_sel_idx].escape_seq;
                                lines[cursor_row].insert(cursor_col, esc);
                                cursor_col += (int)esc.size();
                                modified = true;
                                show_graphics_modal = false;
                                status_msg = "[ Inserido: " + esc + " ]";
                                status_msg_time = std::chrono::steady_clock::now();
                            } else if (mpos.X >= dlg_x + 36 && mpos.X <= dlg_x + 58) {
                                show_graphics_modal = false;
                            }
                        }
                        prev_btn_state = btn;
                        break;
                    }

                    // C. Clique na Barra de Menus Superior (Linha 0)
                    if (mpos.Y == 0) {
                        for (size_t m = 0; m < menus.size(); ++m) {
                            if (mpos.X >= menus[m].x_pos && mpos.X < menus[m].x_pos + menus[m].width) {
                                active_menu_idx = (int)m;
                                active_item_idx = 0;
                                menu_active = true;
                                break;
                            }
                        }
                        prev_btn_state = btn;
                        break;
                    }

                    // D. Botao fechar [×] (Fecha janela no TUI ou sai no CLI)
                    if (mpos.Y == win_y && mpos.X >= win_x + 2 && mpos.X <= win_x + 4) {
                        execute_action(104);
                        prev_btn_state = btn;
                        break;
                    }

                    // E. Botao maximizar/restaurar [▲] / [↕]
                    if (mpos.Y == win_y && mpos.X >= win_x + win_w - 5 && mpos.X <= win_x + win_w - 3) {
                        if (maximized) {
                            maximized = false;
                            win_x = restored_x;
                            win_y = restored_y;
                            win_w = restored_w;
                            win_h = restored_h;
                        } else {
                            restored_x = win_x;
                            restored_y = win_y;
                            restored_w = win_w;
                            restored_h = win_h;
                            maximized = true;
                            win_x = 0;
                            win_y = 1;
                            win_w = term_w;
                            win_h = std::max(3, term_h - 2);
                        }
                        prev_btn_state = btn;
                        break;
                    }

                    // F. Puxador inferior direito para redimensionamento (◢)
                    if (mpos.Y == win_y + win_h - 1 && mpos.X >= win_x + win_w - 2 && mpos.X <= win_x + win_w - 1) {
                        if (!maximized) {
                            is_resizing = true;
                        }
                        prev_btn_state = btn;
                        break;
                    }

                    // G. Clique na Barra de Titulo para Arrastar (EXCLUI regioes dos botoes [×] e [▲])
                    if (mpos.Y == win_y && mpos.X > win_x + 4 && mpos.X < win_x + win_w - 5 && !maximized) {
                        is_dragging = true;
                        drag_offset_x = mpos.X - win_x;
                        drag_offset_y = mpos.Y - win_y;
                        prev_btn_state = btn;
                        break;
                    }

                    // H. Barra vertical de rolagem
                    if (mpos.X == win_x + win_w - 1) {
                        if (mpos.Y == win_y + 1) {
                            cursor_row = std::max(0, cursor_row - 1);
                        } else if (mpos.Y == win_y + win_h - 2) {
                            cursor_row = std::min((int)lines.size() - 1, cursor_row + 1);
                        } else if (mpos.Y > win_y + 1 && mpos.Y < win_y + win_h - 2) {
                            int track_click = mpos.Y - (win_y + 2);
                            if (v_track_len > 0) {
                                float ratio = (float)track_click / (float)v_track_len;
                                cursor_row = (int)(ratio * (total_lines - 1));
                            }
                        }
                        prev_btn_state = btn;
                        break;
                    }

                    // I. Barra horizontal de rolagem
                    if (mpos.Y == win_y + win_h - 1) {
                        if (mpos.X == win_x + 1) {
                            cursor_col = std::max(0, cursor_col - 4);
                        } else if (mpos.X == win_x + win_w - 7) {
                            cursor_col += 4;
                        } else if (mpos.X > win_x + 1 && mpos.X < win_x + win_w - 7) {
                            int track_click = mpos.X - (win_x + 2);
                            if (h_track_len > 0) {
                                float ratio = (float)track_click / (float)h_track_len;
                                cursor_col = (int)(ratio * max_col_observed);
                            }
                        }
                        prev_btn_state = btn;
                        break;
                    }

                    // J. Clique no texto (posiciona cursor e inicia selecao)
                    if (mpos.Y > win_y && mpos.Y < win_y + win_h - 1 &&
                        mpos.X > win_x && mpos.X < win_x + win_w - 1) {
                        int click_r = scroll_row + (mpos.Y - win_y - 1);
                        int click_c = scroll_col + (mpos.X - win_x - 1);
                        cursor_row = std::max(0, std::min((int)lines.size() - 1, click_r));
                        int target_line_len = (int)lines[cursor_row].size();
                        cursor_col = std::max(0, std::min(target_line_len, click_c));

                        has_selection = false;
                        is_mouse_selecting = true;
                        sel_start_row = cursor_row;
                        sel_start_col = cursor_col;
                        sel_end_row = cursor_row;
                        sel_end_col = cursor_col;

                        prev_btn_state = btn;
                        break;
                    }
                }

                prev_btn_state = btn;
                continue;
            }

            // ---------------------------------------------------------
            // EVENTOS DE TECLADO
            // ---------------------------------------------------------
            else if (rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown) {
                needs_render = true;
                WORD vk = rec.Event.KeyEvent.wVirtualKeyCode;
                DWORD ctrl_state = rec.Event.KeyEvent.dwControlKeyState;
                wchar_t wc = rec.Event.KeyEvent.uChar.UnicodeChar;

                bool is_ctrl = (ctrl_state & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
                bool is_alt  = (ctrl_state & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0;
                bool is_shift = (ctrl_state & SHIFT_PRESSED) != 0;

                auto delete_selection = [&]() -> bool {
                    if (!has_selection) return false;
                    int r1 = sel_start_row, c1 = sel_start_col;
                    int r2 = sel_end_row, c2 = sel_end_col;
                    if (r1 > r2 || (r1 == r2 && c1 > c2)) {
                        std::swap(r1, r2);
                        std::swap(c1, c2);
                    }
                    if (r1 < 0) r1 = 0;
                    if (r1 >= (int)lines.size()) r1 = (int)lines.size() - 1;
                    if (r2 < 0) r2 = 0;
                    if (r2 >= (int)lines.size()) r2 = (int)lines.size() - 1;
                    c1 = std::max(0, std::min((int)lines[r1].size(), c1));
                    c2 = std::max(0, std::min((int)lines[r2].size(), c2));

                    if (r1 == r2) {
                        if (c1 < c2) {
                            lines[r1].erase(c1, c2 - c1);
                        }
                    } else {
                        std::string prefix = lines[r1].substr(0, c1);
                        std::string suffix = lines[r2].substr(c2);
                        lines[r1] = prefix + suffix;
                        lines.erase(lines.begin() + r1 + 1, lines.begin() + r2 + 1);
                    }
                    cursor_row = r1;
                    cursor_col = c1;
                    has_selection = false;
                    modified = true;
                    return true;
                };

                // 1. Popup de salvamento
                if (show_exit_dlg) {
                    if (vk == VK_ESCAPE || wc == 'c' || wc == 'C') {
                        show_exit_dlg = false;
                    } else if (wc == 's' || wc == 'S' || (vk == VK_RETURN && exit_dlg_choice == 0)) {
                        execute_action(102); // Salva
                        exit_return_code = (exit_dlg_is_close_window ? 0 : 1);
                        running = false;
                    } else if (wc == 'n' || wc == 'N' || (vk == VK_RETURN && exit_dlg_choice == 1)) {
                        exit_return_code = (exit_dlg_is_close_window ? 0 : 1);
                        running = false;
                    } else if (vk == VK_LEFT) {
                        exit_dlg_choice = (exit_dlg_choice + 2) % 3;
                    } else if (vk == VK_RIGHT || vk == VK_TAB) {
                        exit_dlg_choice = (exit_dlg_choice + 1) % 3;
                    }
                    break;
                }

                // 2. Modal de Erro de Compilacao
                if (show_compile_error_modal) {
                    if (vk == VK_RETURN || vk == VK_ESCAPE || vk == VK_SPACE) {
                        show_compile_error_modal = false;
                        if (compile_error_line > 0 && compile_error_line <= (int)lines.size()) {
                            cursor_row = compile_error_line - 1;
                            cursor_col = 0;
                            scroll_row = std::max(0, cursor_row - inner_h / 2);
                        }
                    }
                    break;
                }

                // 3. Modal de Sucesso de Compilacao
                if (show_compile_success_modal) {
                    if (vk == VK_RETURN || vk == VK_ESCAPE || vk == VK_SPACE) {
                        show_compile_success_modal = false;
                    }
                    break;
                }

                // 4. Modal Sobre (About)
                if (show_about_modal) {
                    if (vk == VK_RETURN || vk == VK_ESCAPE || vk == VK_SPACE) {
                        show_about_modal = false;
                    }
                    break;
                }

                // 5. Modal de Ajuda
                if (show_help_modal) {
                    if (vk == VK_RETURN || vk == VK_ESCAPE || vk == VK_SPACE) {
                        show_help_modal = false;
                    }
                    break;
                }

                // 6. Modal de Opcoes do Compilador (F7 / Ctrl+T)
                if (show_compiler_opts_modal) {
                    if (vk == VK_ESCAPE) {
                        show_compiler_opts_modal = false;
                    } else if (vk == VK_UP) {
                        compiler_opts_focus = (compiler_opts_focus + 2) % 3;
                    } else if (vk == VK_DOWN || vk == VK_TAB) {
                        compiler_opts_focus = (compiler_opts_focus + 1) % 3;
                    } else if (vk == VK_SPACE || vk == VK_RETURN) {
                        if (compiler_opts_focus == 0) {
                            bool cur = cfg.get_compiler_labels_mode();
                            cfg.set_compiler_labels_mode(!cur);
                        } else if (vk == VK_RETURN) {
                            show_compiler_opts_modal = false;
                            status_msg = "[ Parametros Gravados com Sucesso! ]";
                            status_msg_time = std::chrono::steady_clock::now();
                        }
                    } else if (vk == VK_LEFT) {
                        if (compiler_opts_focus == 1) {
                            int s = cfg.get_compiler_start_line();
                            cfg.set_compiler_start_line(std::max(0, s - 5));
                        } else if (compiler_opts_focus == 2) {
                            int inc = cfg.get_compiler_line_increment();
                            cfg.set_compiler_line_increment(std::max(1, inc - 1));
                        }
                    } else if (vk == VK_RIGHT) {
                        if (compiler_opts_focus == 1) {
                            int s = cfg.get_compiler_start_line();
                            cfg.set_compiler_start_line(std::min(9990, s + 5));
                        } else if (compiler_opts_focus == 2) {
                            int inc = cfg.get_compiler_line_increment();
                            cfg.set_compiler_line_increment(std::min(100, inc + 1));
                        }
                    }
                    break;
                }

                // 7. Modal de Caracteres Graficos (F4 / Ctrl+G)
                if (show_graphics_modal) {
                    if (vk == VK_ESCAPE) {
                        show_graphics_modal = false;
                    } else if (vk == VK_UP) {
                        graphics_sel_idx = (graphics_sel_idx + 23) % 24;
                    } else if (vk == VK_DOWN) {
                        graphics_sel_idx = (graphics_sel_idx + 1) % 24;
                    } else if (vk == VK_LEFT || vk == VK_RIGHT) {
                        graphics_sel_idx = (graphics_sel_idx + 12) % 24;
                    } else if (vk == VK_RETURN) {
                        std::string esc = gfx_table[graphics_sel_idx].escape_seq;
                        lines[cursor_row].insert(cursor_col, esc);
                        cursor_col += (int)esc.size();
                        modified = true;
                        show_graphics_modal = false;
                        status_msg = "[ Inserido: " + esc + " ]";
                        status_msg_time = std::chrono::steady_clock::now();
                    }
                    break;
                }

                // 8. Navegacao na Barra de Menus (se menu_active == true)
                if (menu_active) {
                    if (vk == VK_F10 || vk == VK_ESCAPE) {
                        menu_active = false;
                    } else if (vk == VK_LEFT) {
                        active_menu_idx = (active_menu_idx + (int)menus.size() - 1) % (int)menus.size();
                        active_item_idx = 0;
                    } else if (vk == VK_RIGHT) {
                        active_menu_idx = (active_menu_idx + 1) % (int)menus.size();
                        active_item_idx = 0;
                    } else if (vk == VK_UP) {
                        int num_items = (int)menus[active_menu_idx].items.size();
                        active_item_idx = (active_item_idx + num_items - 1) % num_items;
                        redraw_dropdown_fast();
                        needs_render = false;
                        break;
                    } else if (vk == VK_DOWN) {
                        int num_items = (int)menus[active_menu_idx].items.size();
                        active_item_idx = (active_item_idx + 1) % num_items;
                        redraw_dropdown_fast();
                        needs_render = false;
                        break;
                    } else if (vk == VK_RETURN) {
                        int act_id = menus[active_menu_idx].items[active_item_idx].id;
                        menu_active = false;
                        execute_action(act_id);
                    }
                    break;
                }

                // Atalhos de Menu (Alt+Letra):
                if (is_alt && (vk == 'A' || vk == 'a')) {
                    menu_active = true;
                    active_menu_idx = 0;
                    active_item_idx = 0;
                    break;
                }
                if (is_alt && (vk == 'E' || vk == 'e')) {
                    menu_active = true;
                    active_menu_idx = 1;
                    active_item_idx = 0;
                    break;
                }
                if (is_alt && (vk == 'C' || vk == 'c')) {
                    menu_active = true;
                    active_menu_idx = 2;
                    active_item_idx = 0;
                    break;
                }
                if (is_alt && (vk == 'O' || vk == 'o')) {
                    menu_active = true;
                    active_menu_idx = 3;
                    active_item_idx = 0;
                    break;
                }
                if (is_alt && (vk == 'H' || vk == 'h' || vk == 'J' || vk == 'j')) {
                    menu_active = true;
                    active_menu_idx = 4;
                    active_item_idx = 0;
                    break;
                }

                // F10: Ativar/Desativar Barra de Menus
                if (vk == VK_F10) {
                    menu_active = !menu_active;
                    active_menu_idx = 0;
                    active_item_idx = 0;
                    break;
                }

                // F1: Ajuda
                if (vk == VK_F1) {
                    execute_action(501);
                    break;
                }

                // F2 / Ctrl+S: Salvar ASCII .BAS
                if (vk == VK_F2 || (is_ctrl && (vk == 'S' || vk == 's'))) {
                    execute_action(102);
                    break;
                }

                // Ctrl+N: Novo Arquivo
                if (is_ctrl && (vk == 'N' || vk == 'n')) {
                    execute_action(101);
                    break;
                }

                // F4 / Ctrl+G: Seletor de Caracteres Graficos ZX-81
                if (vk == VK_F4 || (is_ctrl && (vk == 'G' || vk == 'g'))) {
                    execute_action(201);
                    break;
                }

                // Ctrl+I: Inverter Video ZX-81 (%)
                if (is_ctrl && (vk == 'I' || vk == 'i')) {
                    execute_action(202);
                    break;
                }

                // F5: Maximizar / Restaurar janela
                if (vk == VK_F5) {
                    if (maximized) {
                        maximized = false;
                        win_x = restored_x;
                        win_y = restored_y;
                        win_w = restored_w;
                        win_h = restored_h;
                    } else {
                        restored_x = win_x;
                        restored_y = win_y;
                        restored_w = win_w;
                        restored_h = win_h;
                        maximized = true;
                        win_x = 0;
                        win_y = 1;
                        win_w = term_w;
                        win_h = std::max(3, term_h - 2);
                    }
                    break;
                }

                // F7 / Ctrl+T: Parametros do Compilador ZXText2P
                if (vk == VK_F7 || (is_ctrl && (vk == 'T' || vk == 't'))) {
                    execute_action(302);
                    break;
                }

                // F9 / Ctrl+B: Compilar para Formato .P do ZX-81
                if (vk == VK_F9 || (is_ctrl && (vk == 'B' || vk == 'b'))) {
                    execute_action(301);
                    break;
                }

                // Ctrl+F4: Fechar Janela (104)
                if (is_ctrl && vk == VK_F4) {
                    execute_action(104);
                    break;
                }

                // ESC: Cancela selecao se ativa; no TUI fecha janela (104); no CLI sai (103)
                if (vk == VK_ESCAPE) {
                    if (has_selection) {
                        has_selection = false;
                        break;
                    }
                    if (presentation == EditorPresentation::TUI_INTEGRATED) {
                        execute_action(104);
                    } else {
                        execute_action(103);
                    }
                    break;
                }

                // Alt+X ou Ctrl+Q: Sair da TUI / Sair da aplicacao (103)
                if ((is_alt && (vk == 'X' || vk == 'x')) || (is_ctrl && (vk == 'Q' || vk == 'q'))) {
                    execute_action(103);
                    break;
                }

                // Navegacao no texto
                if (vk == VK_UP || vk == VK_DOWN || vk == VK_LEFT || vk == VK_RIGHT ||
                    vk == VK_HOME || vk == VK_END || vk == VK_PRIOR || vk == VK_NEXT) {

                    if (is_shift && !has_selection) {
                        has_selection = true;
                        sel_start_row = cursor_row;
                        sel_start_col = cursor_col;
                    } else if (!is_shift && has_selection) {
                        has_selection = false;
                    }

                    if (vk == VK_UP) {
                        cursor_row = std::max(0, cursor_row - 1);
                    } else if (vk == VK_DOWN) {
                        cursor_row = std::min((int)lines.size() - 1, cursor_row + 1);
                    } else if (vk == VK_LEFT) {
                        if (cursor_col > 0) {
                            cursor_col--;
                        } else if (cursor_row > 0) {
                            cursor_row--;
                            cursor_col = (int)lines[cursor_row].size();
                        }
                    } else if (vk == VK_RIGHT) {
                        if (cursor_col < (int)lines[cursor_row].size()) {
                            cursor_col++;
                        } else if (cursor_row + 1 < (int)lines.size()) {
                            cursor_row++;
                            cursor_col = 0;
                        }
                    } else if (vk == VK_HOME) {
                        cursor_col = 0;
                    } else if (vk == VK_END) {
                        cursor_col = (int)lines[cursor_row].size();
                    } else if (vk == VK_PRIOR) {
                        cursor_row = std::max(0, cursor_row - inner_h);
                    } else if (vk == VK_NEXT) {
                        cursor_row = std::min((int)lines.size() - 1, cursor_row + inner_h);
                    }

                    cursor_col = std::min(cursor_col, (int)lines[cursor_row].size());

                    if (is_shift) {
                        sel_end_row = cursor_row;
                        sel_end_col = cursor_col;
                        if (sel_start_row == sel_end_row && sel_start_col == sel_end_col) {
                            has_selection = false;
                        }
                    }
                }
                // Backspace
                else if (vk == VK_BACK) {
                    if (!delete_selection()) {
                        if (cursor_col > 0) {
                            lines[cursor_row].erase(cursor_col - 1, 1);
                            cursor_col--;
                            modified = true;
                        } else if (cursor_row > 0) {
                            int prev_len = (int)lines[cursor_row - 1].size();
                            lines[cursor_row - 1] += lines[cursor_row];
                            lines.erase(lines.begin() + cursor_row);
                            cursor_row--;
                            cursor_col = prev_len;
                            modified = true;
                        }
                    }
                }
                // Delete
                else if (vk == VK_DELETE) {
                    if (!delete_selection()) {
                        if (cursor_col < (int)lines[cursor_row].size()) {
                            lines[cursor_row].erase(cursor_col, 1);
                            modified = true;
                        } else if (cursor_row + 1 < (int)lines.size()) {
                            lines[cursor_row] += lines[cursor_row + 1];
                            lines.erase(lines.begin() + cursor_row + 1);
                            modified = true;
                        }
                    }
                }
                // Enter
                else if (vk == VK_RETURN) {
                    delete_selection();
                    std::string current = lines[cursor_row];
                    std::string before = current.substr(0, cursor_col);
                    std::string after = current.substr(cursor_col);

                    lines[cursor_row] = before;
                    lines.insert(lines.begin() + cursor_row + 1, after);

                    cursor_row++;
                    cursor_col = 0;
                    modified = true;
                }
                // Tab
                else if (vk == VK_TAB) {
                    delete_selection();
                    lines[cursor_row].insert(cursor_col, "    ");
                    cursor_col += 4;
                    modified = true;
                }
                // Caractere digitavel normal
                else if (wc >= 32 && wc != 127) {
                    delete_selection();
                    char c = (char)wc;
                    lines[cursor_row].insert(lines[cursor_row].begin() + cursor_col, c);
                    cursor_col++;
                    modified = true;
                }
            }
        }
#else
        running = false;
#endif
    }

    // Restaurar console
#ifdef _WIN32
    if (hOut != INVALID_HANDLE_VALUE) {
        if (has_orig_cci) {
            SetConsoleCursorInfo(hOut, &orig_cci);
        }
        SetConsoleMode(hOut, prev_out_mode);
    }
    if (hIn != INVALID_HANDLE_VALUE) {
        SetConsoleMode(hIn, prev_in_mode);
    }
#endif

    std::cout << "\x1b[0 q" << Colors::RESET << Colors::SHOW_CURSOR;
    std::cout << "\x1b[2J" << Colors::HOME << std::flush;

    return exit_return_code;
}

} // namespace editor
} // namespace zx81ide
