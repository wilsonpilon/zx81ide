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
std::string render_editor_line(const std::string& raw_line, int scroll_col, int inner_w, const EditorTheme& theme) {
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
    out << theme.text_fg << theme.text_bg;

    for (int col = 0; col < inner_w; ++col) {
        int doc_idx = scroll_col + col;
        if (doc_idx < (int)N) {
            SyntaxTag t = tags[doc_idx];
            if (t != cur_tag) {
                cur_tag = t;
                out << get_syntax_color(cur_tag, theme);
            }
            out << expanded[doc_idx];
        } else {
            if (cur_tag != SyntaxTag::NORMAL) {
                cur_tag = SyntaxTag::NORMAL;
                out << theme.text_fg << theme.text_bg;
            }
            out << ' ';
        }
    }
    out << theme.text_fg << theme.text_bg;
    return out.str();
}

} // anonymous namespace

int run_text_editor(const std::string& file_path, EditorPresentation presentation, EditorStyle style) {
    (void)presentation;
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

    if (hOut != INVALID_HANDLE_VALUE) {
        GetConsoleMode(hOut, &prev_out_mode);
        SetConsoleMode(hOut, prev_out_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    if (hIn != INVALID_HANDLE_VALUE) {
        GetConsoleMode(hIn, &prev_in_mode);
        SetConsoleMode(hIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
    }

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::string display_name = extract_file_name(file_path);
    if (display_name.empty()) {
        display_name = "NONAME.BAS";
    }

    // Estado do Editor
    int cursor_row = 0;
    int cursor_col = 0;
    int scroll_row = 0;
    int scroll_col = 0;
    bool modified = false;
    bool running = true;
    bool maximized = false;
    std::string status_msg = is_new_file ? "[ Novo Arquivo ]" : "[ Arquivo Carregado ]";
    auto status_msg_time = std::chrono::steady_clock::now();

    // Dimensoes da janela (padrao inicial: 80x25 uteis para escrita + moldura = 82x27)
    int term_w = 80;
    int term_h = 25;
    get_terminal_size(term_w, term_h);

    int win_w = 82;
    int win_h = 27;

    if (win_w > term_w) win_w = term_w;
    if (win_h > term_h - 1) win_h = term_h - 1;

    int win_x = (term_w > win_w) ? (term_w - win_w) / 2 : 0;
    int win_y = (term_h - 1 > win_h) ? (term_h - 1 - win_h) / 2 : 0;

    int restored_x = win_x;
    int restored_y = win_y;
    int restored_w = win_w;
    int restored_h = win_h;

    bool is_dragging = false;
    int drag_offset_x = 0;
    int drag_offset_y = 0;
    bool is_resizing = false;

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

    // Loop Principal de Edicao
    while (running) {
        get_terminal_size(term_w, term_h);

        if (maximized) {
            win_x = 0;
            win_y = 0;
            win_w = term_w;
            win_h = term_h - 1;
        } else {
            if (win_w > term_w) win_w = term_w;
            if (win_h > term_h - 1) win_h = term_h - 1;
            if (win_x + win_w > term_w) win_x = std::max(0, term_w - win_w);
            if (win_y + win_h > term_h - 1) win_y = std::max(0, term_h - 1 - win_h);
        }

        int inner_w = std::max(1, win_w - 2);
        int inner_h = std::max(1, win_h - 2);

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

        int total_lines = (int)lines.size();
        int v_track_len = std::max(1, inner_h - 2);
        int v_thumb_pos = 0;
        if (total_lines > 1) {
            float v_ratio = (float)cursor_row / (float)(total_lines - 1);
            v_thumb_pos = (int)(v_ratio * (v_track_len - 1));
        }

        int max_col_observed = 80;
        for (const auto& l : lines) {
            if ((int)l.size() > max_col_observed) max_col_observed = (int)l.size();
        }
        int h_track_len = std::max(1, inner_w - 8);
        int h_thumb_pos = 0;
        if (max_col_observed > 0) {
            float h_ratio = (float)cursor_col / (float)max_col_observed;
            h_thumb_pos = (int)(h_ratio * (h_track_len - 1));
        }

        // Renderizar linha por linha da tela do terminal
        for (int y = 0; y < term_h - 1; ++y) {
            if (y < win_y || y >= win_y + win_h) {
                buf << theme.desktop_fg << theme.desktop_bg;
                for (int x = 0; x < term_w; ++x) {
                    buf << theme.desktop_char;
                }
                buf << Colors::RESET << "\n";
                continue;
            }

            if (win_x > 0) {
                buf << theme.desktop_fg << theme.desktop_bg;
                for (int x = 0; x < win_x; ++x) {
                    buf << theme.desktop_char;
                }
            }

            // LINHA 0: BARRA DE TITULO / BORDA SUPERIOR
            if (y == win_y) {
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
            }
            // LINHAS 1 A H-2: INTERIOR DO EDITOR E BARRA VERTICAL
            else if (y > win_y && y < win_y + win_h - 1) {
                int vr = y - win_y - 1;
                int doc_line_idx = scroll_row + vr;

                buf << theme.frame_fg << theme.frame_bg << "║";

                // Renderizacao da linha com Destaque de Sintaxe
                if (doc_line_idx < (int)lines.size()) {
                    buf << render_editor_line(lines[doc_line_idx], scroll_col, inner_w, theme);
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
            }
            // LINHA H-1: BORDA INFERIOR E BARRA HORIZONTAL
            else if (y == win_y + win_h - 1) {
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

            int right_desk = term_w - (win_x + win_w);
            if (right_desk > 0) {
                buf << theme.desktop_fg << theme.desktop_bg;
                for (int x = 0; x < right_desk; ++x) {
                    buf << theme.desktop_char;
                }
            }

            buf << Colors::RESET << "\n";
        }

        // LINHA INFERIOR DO TERMINAL: BARRA DE ATALHOS / STATUS DO SISTEMA
        {
            buf << theme.status_fg << theme.status_bg;
            std::string help_bar = " F1:Ajuda  F2:Salvar  F4:Graficos  F5:Zoom  F7:Opcoes  F9:Compilar.P  Esc:Sair | L:" + 
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
        // RENDERIZACAO DE MODAIS
        // =============================================================

        // 1. MODAL DE CONFIRMACAO DE SAIDA
        if (show_exit_dlg) {
            int dlg_w = 46;
            int dlg_h = 7;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            buf << "\x1b[" << (dlg_y + 1) << ";" << (dlg_x + 1) << "H";
            buf << theme.dlg_border_fg << theme.dlg_border_bg << "╔════════════════════════════════════════════╗";

            buf << "\x1b[" << (dlg_y + 2) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "               Salvar Arquivo               " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 3) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "  O arquivo foi modificado. Salvar agora?   " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 4) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "                                            " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 5) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_body_bg << "   ";
            if (exit_dlg_choice == 0) buf << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg << "[ Sim (S) ]";
            else buf << theme.dlg_btn_fg << theme.dlg_btn_bg << "[ Sim (S) ]";
            buf << theme.dlg_body_bg << "  ";
            if (exit_dlg_choice == 1) buf << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg << "[ Nao (N) ]";
            else buf << theme.dlg_btn_fg << theme.dlg_btn_bg << "[ Nao (N) ]";
            buf << theme.dlg_body_bg << "  ";
            if (exit_dlg_choice == 2) buf << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg << "[ Cancelar (C) ]";
            else buf << theme.dlg_btn_fg << theme.dlg_btn_bg << "[ Cancelar (C) ]";
            buf << theme.dlg_body_bg << "   " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 6) << ";" << (dlg_x + 1) << "H";
            buf << "╚════════════════════════════════════════════╝" << Colors::RESET;
        }

        // 2. MODAL SELETOR DE CARACTERES GRAFICOS (F4 / Ctrl+G)
        const auto& gfx_table = compiler::ZXText2P::get_graphics_table();
        if (show_graphics_modal) {
            int dlg_w = 64;
            int dlg_h = 18;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            buf << "\x1b[" << (dlg_y + 1) << ";" << (dlg_x + 1) << "H";
            buf << theme.dlg_border_fg << theme.dlg_border_bg << "╔═════════[ Caracteres Graficos do ZX-81 (F4) ]═════════╗";

            buf << "\x1b[" << (dlg_y + 2) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << " Navegue com as setas e tecle Enter para inserir o escape:   " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            // 12 linhas de itens em 2 colunas
            for (int row = 0; row < 12; ++row) {
                int idx1 = row;
                int idx2 = row + 12;

                buf << "\x1b[" << (dlg_y + 3 + row) << ";" << (dlg_x + 1) << "H";
                buf << "║" << theme.dlg_body_bg << " ";

                // Coluna 1
                if (idx1 < (int)gfx_table.size()) {
                    bool sel1 = (graphics_sel_idx == idx1);
                    std::string label1 = " [" + gfx_table[idx1].glyph_utf8 + "] " + gfx_table[idx1].escape_seq + " ";
                    if (label1.size() < 12) label1.append(12 - label1.size(), ' ');

                    if (sel1) buf << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg << label1 << theme.dlg_body_bg;
                    else buf << theme.dlg_text_fg << label1;
                } else {
                    buf << std::string(12, ' ');
                }

                buf << "   │  ";

                // Coluna 2
                if (idx2 < (int)gfx_table.size()) {
                    bool sel2 = (graphics_sel_idx == idx2);
                    std::string label2 = " [" + gfx_table[idx2].glyph_utf8 + "] " + gfx_table[idx2].escape_seq + " ";
                    if (label2.size() < 12) label2.append(12 - label2.size(), ' ');

                    if (sel2) buf << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg << label2 << theme.dlg_body_bg;
                    else buf << theme.dlg_text_fg << label2;
                } else {
                    buf << std::string(12, ' ');
                }

                buf << "        " << theme.dlg_border_fg << theme.dlg_border_bg << "║";
            }

            // Linha com descricao do selecionado
            std::string desc_line = " " + gfx_table[graphics_sel_idx].description + 
                                    " (ZX: " + std::to_string(gfx_table[graphics_sel_idx].zx_code) + ") ";
            if ((int)desc_line.size() > dlg_w - 4) desc_line = desc_line.substr(0, dlg_w - 4);
            int desc_pad = (dlg_w - 2) - (int)desc_line.size();

            buf << "\x1b[" << (dlg_y + 15) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg << desc_line 
                << theme.dlg_body_bg << std::string(std::max(0, desc_pad), ' ') << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 16) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_body_bg << "    [ Enter: Inserir no Editor ]        [ Esc: Cancelar ]    " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 17) << ";" << (dlg_x + 1) << "H";
            buf << "╚══════════════════════════════════════════════════════════╝" << Colors::RESET;
        }

        // 3. MODAL DE ERRO DE COMPILACAO
        if (show_compile_error_modal) {
            int dlg_w = 60;
            int dlg_h = 8;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            buf << "\x1b[" << (dlg_y + 1) << ";" << (dlg_x + 1) << "H";
            buf << theme.dlg_border_fg << theme.dlg_border_bg << "╔══════════[ Erro de Compilacao ZXText2P ]══════════╗";

            buf << "\x1b[" << (dlg_y + 2) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "  Falha ao gerar binario .P do Sinclair ZX-81:          " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            std::string err_line_str = "  Linha " + std::to_string(compile_error_line) + ": " + compile_error_msg;
            if ((int)err_line_str.size() > dlg_w - 4) err_line_str = err_line_str.substr(0, dlg_w - 4);
            int err_pad = (dlg_w - 2) - (int)err_line_str.size();

            buf << "\x1b[" << (dlg_y + 3) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg << err_line_str << theme.dlg_body_bg << std::string(std::max(0, err_pad), ' ') << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 4) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "                                                          " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 5) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_body_bg << "      [ Enter / Esc: Ir ate a Linha do Erro ]             " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 6) << ";" << (dlg_x + 1) << "H";
            buf << "╚══════════════════════════════════════════════════════════╝" << Colors::RESET;
        }

        // 4. MODAL DE SUCESSO DE COMPILACAO
        if (show_compile_success_modal) {
            int dlg_w = 58;
            int dlg_h = 7;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            buf << "\x1b[" << (dlg_y + 1) << ";" << (dlg_x + 1) << "H";
            buf << theme.dlg_border_fg << theme.dlg_border_bg << "╔═════════[ Compilacao Concluida (.P) ]═════════╗";

            buf << "\x1b[" << (dlg_y + 2) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "  Programa compilado e salvo com sucesso em formato .P! " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            std::string succ_line = "  " + compile_success_msg;
            if ((int)succ_line.size() > dlg_w - 4) succ_line = succ_line.substr(0, dlg_w - 4);
            int succ_pad = (dlg_w - 2) - (int)succ_line.size();

            buf << "\x1b[" << (dlg_y + 3) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << succ_line << std::string(std::max(0, succ_pad), ' ') << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 4) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_body_bg << "              [ Enter / Espaco / Esc: OK ]            " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 5) << ";" << (dlg_x + 1) << "H";
            buf << "╚════════════════════════════════════════════════════════╝" << Colors::RESET;
        }

        // 5. MODAL DE AJUDA DO EDITOR (F1)
        if (show_help_modal) {
            int dlg_w = 62;
            int dlg_h = 16;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            buf << "\x1b[" << (dlg_y + 1) << ";" << (dlg_x + 1) << "H";
            buf << theme.dlg_border_fg << theme.dlg_border_bg << "╔═════════════[ Ajuda do Editor ZX-81 (F1) ]═════════════╗";

            buf << "\x1b[" << (dlg_y + 2) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "  Atalhos Principais do Editor de Texto:                    " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 3) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "    F1          : Exibe esta tela de ajuda                  " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 4) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "    F2 / Ctrl+S : Salva o arquivo ASCII (.BAS) no disco     " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 5) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "    F4 / Ctrl+G : Seletor de Caracteres Graficos do ZX-81   " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 6) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "    F5          : Maximiza / Restaura tamanho da janela     " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 7) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "    F7 / Ctrl+T : Configuracoes do Compilador ZXText2P      " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 8) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "    F9 / Ctrl+B : Compila o fonte para binario ZX-81 (.P)   " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 9) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "    Esc / Alt+X : Encerra e fecha a janela do editor        " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 10) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "                                                            " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 11) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "  Recursos de Edicao:                                       " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 12) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "    - Destaque de Sintaxe para comandos, numeros e strings  " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 13) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "    - Suporte a Mouse: Arrastar titulo, Redimensionar (◢)   " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 14) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_body_bg << "               [ Pressione Enter ou Esc para Fechar ]         " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 15) << ";" << (dlg_x + 1) << "H";
            buf << "╚════════════════════════════════════════════════════════════╝" << Colors::RESET;
        }

        // 6. MODAL DE OPCOES DO COMPILADOR (F7 / Ctrl+T)
        if (show_compiler_opts_modal) {
            int dlg_w = 56;
            int dlg_h = 10;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            bool labels_mode = cfg.get_compiler_labels_mode();
            int start_line = cfg.get_compiler_start_line();
            int line_inc = cfg.get_compiler_line_increment();

            buf << "\x1b[" << (dlg_y + 1) << ";" << (dlg_x + 1) << "H";
            buf << theme.dlg_border_fg << theme.dlg_border_bg << "╔════════[ Parametros do Compilador ZXText2P ]════════╗";

            buf << "\x1b[" << (dlg_y + 2) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_text_fg << theme.dlg_body_bg << "  Altere com Setas / Espaco e tecle Enter para gravar:  " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            // Opcao 0: Modo Labels
            buf << "\x1b[" << (dlg_y + 3) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_body_bg << "    Modo Labels (@label): ";
            std::string lbl_val = labels_mode ? "[ Ativado (Sim) ]" : "[ Numeros Explicitos ]";
            if (compiler_opts_focus == 0) buf << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg << lbl_val << theme.dlg_body_bg;
            else buf << theme.dlg_btn_fg << theme.dlg_btn_bg << lbl_val;
            buf << "      " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            // Opcao 1: Linha Inicial
            buf << "\x1b[" << (dlg_y + 4) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_body_bg << "    Linha Inicial (-s)  : ";
            std::string s_val = "< " + std::to_string(start_line) + " >";
            if (compiler_opts_focus == 1) buf << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg << s_val << theme.dlg_body_bg;
            else buf << theme.dlg_btn_fg << theme.dlg_btn_bg << s_val;
            buf << "                  " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            // Opcao 2: Incremento
            buf << "\x1b[" << (dlg_y + 5) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_body_bg << "    Incremento    (-i)  : ";
            std::string i_val = "< " + std::to_string(line_inc) + " >";
            if (compiler_opts_focus == 2) buf << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg << i_val << theme.dlg_body_bg;
            else buf << theme.dlg_btn_fg << theme.dlg_btn_bg << i_val;
            buf << "                  " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 6) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_body_bg << "                                                        " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 7) << ";" << (dlg_x + 1) << "H";
            buf << "║" << theme.dlg_body_bg << "           [ Enter: Salvar ]        [ Esc: Fechar ]     " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 8) << ";" << (dlg_x + 1) << "H";
            buf << "╚══════════════════════════════════════════════════════╝" << Colors::RESET;
        }

        // Posicionar cursor real do console na posicao de digitacao
        bool any_modal_open = show_exit_dlg || show_graphics_modal || show_compile_error_modal || 
                              show_compile_success_modal || show_help_modal || show_compiler_opts_modal;

        if (!any_modal_open) {
            int cursor_screen_x = win_x + 1 + (cursor_col - scroll_col) + 1;
            int cursor_screen_y = win_y + 1 + (cursor_row - scroll_row) + 1;
            if (cursor_screen_x >= win_x + 2 && cursor_screen_x <= win_x + inner_w + 1 &&
                cursor_screen_y >= win_y + 2 && cursor_screen_y <= win_y + inner_h + 1) {
                buf << "\x1b[" << cursor_screen_y << ";" << cursor_screen_x << "H" << Colors::SHOW_CURSOR;
            }
        }

        std::cout << buf.str() << std::flush;

        // =============================================================
        // CAPTURA DE EVENTOS (TECLADO E MOUSE)
        // =============================================================
#ifdef _WIN32
        INPUT_RECORD ir[16];
        DWORD num_events = 0;
        if (!ReadConsoleInputW(hIn, ir, 16, &num_events) || num_events == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
            continue;
        }

        for (DWORD e = 0; e < num_events; ++e) {
            auto& rec = ir[e];

            // ---------------------------------------------------------
            // EVENTOS DE MOUSE
            // ---------------------------------------------------------
            if (rec.EventType == MOUSE_EVENT) {
                COORD mpos = rec.Event.MouseEvent.dwMousePosition;
                DWORD btn = rec.Event.MouseEvent.dwButtonState;
                DWORD flags = rec.Event.MouseEvent.dwEventFlags;

                if (flags & MOUSE_WHEELED) {
                    short delta = (short)HIWORD(rec.Event.MouseEvent.dwButtonState);
                    if (delta > 0) {
                        cursor_row = std::max(0, cursor_row - 3);
                    } else if (delta < 0) {
                        cursor_row = std::min((int)lines.size() - 1, cursor_row + 3);
                    }
                    continue;
                }

                if (btn & FROM_LEFT_1ST_BUTTON_PRESSED) {
                    if (show_exit_dlg) {
                        int dlg_w = 46;
                        int dlg_h = 7;
                        int dlg_x = std::max(0, (term_w - dlg_w) / 2);
                        int dlg_y = std::max(0, (term_h - dlg_h) / 2);

                        if (mpos.Y == dlg_y + 4) {
                            if (mpos.X >= dlg_x + 3 && mpos.X <= dlg_x + 13) {
                                std::ofstream out(file_path, std::ios::out);
                                for (size_t i = 0; i < lines.size(); ++i) {
                                    out << lines[i];
                                    if (i + 1 < lines.size()) out << "\n";
                                }
                                out.close();
                                running = false;
                            } else if (mpos.X >= dlg_x + 16 && mpos.X <= dlg_x + 26) {
                                running = false;
                            } else if (mpos.X >= dlg_x + 29 && mpos.X <= dlg_x + 43) {
                                show_exit_dlg = false;
                            }
                        }
                        continue;
                    }

                    if (show_compile_error_modal) {
                        show_compile_error_modal = false;
                        if (compile_error_line > 0 && compile_error_line <= (int)lines.size()) {
                            cursor_row = compile_error_line - 1;
                            cursor_col = 0;
                            scroll_row = std::max(0, cursor_row - inner_h / 2);
                        }
                        continue;
                    }

                    if (show_compile_success_modal) {
                        show_compile_success_modal = false;
                        continue;
                    }

                    if (show_help_modal) {
                        show_help_modal = false;
                        continue;
                    }

                    if (show_compiler_opts_modal) {
                        show_compiler_opts_modal = false;
                        continue;
                    }

                    if (show_graphics_modal) {
                        int dlg_w = 64;
                        int dlg_h = 18;
                        int dlg_x = std::max(0, (term_w - dlg_w) / 2);
                        int dlg_y = std::max(0, (term_h - dlg_h) / 2);

                        // Clique nas colunas de glifos
                        if (mpos.Y >= dlg_y + 2 && mpos.Y <= dlg_y + 13) {
                            int r = mpos.Y - (dlg_y + 2);
                            if (mpos.X >= dlg_x + 2 && mpos.X <= dlg_x + 18) {
                                graphics_sel_idx = r;
                            } else if (mpos.X >= dlg_x + 22 && mpos.X <= dlg_x + 38) {
                                graphics_sel_idx = r + 12;
                            }
                        } else if (mpos.Y == dlg_y + 15) {
                            if (mpos.X >= dlg_x + 4 && mpos.X <= dlg_x + 32) {
                                // Inserir
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
                        continue;
                    }

                    // 1. Botao fechar [×]
                    if (mpos.Y == win_y && mpos.X >= win_x + 2 && mpos.X <= win_x + 4) {
                        if (modified) {
                            show_exit_dlg = true;
                            exit_dlg_choice = 0;
                        } else {
                            running = false;
                        }
                        continue;
                    }

                    // 2. Botao maximizar/restaurar
                    if (mpos.Y == win_y && mpos.X >= win_x + win_w - 5 && mpos.X <= win_x + win_w - 3) {
                        maximized = !maximized;
                        if (!maximized) {
                            win_x = restored_x;
                            win_y = restored_y;
                            win_w = restored_w;
                            win_h = restored_h;
                        }
                        continue;
                    }

                    // 3. Juncao inferior direita (◢)
                    if (mpos.Y == win_y + win_h - 1 && mpos.X == win_x + win_w - 1) {
                        if (!maximized) {
                            is_resizing = true;
                        }
                        continue;
                    }

                    // 4. Barra vertical
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
                        continue;
                    }

                    // 5. Barra horizontal
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
                        continue;
                    }

                    // 6. Arrastar barra de titulo
                    if (mpos.Y == win_y && !maximized) {
                        is_dragging = true;
                        drag_offset_x = mpos.X - win_x;
                        drag_offset_y = mpos.Y - win_y;
                        continue;
                    }

                    // 7. Clique no texto
                    if (mpos.Y > win_y && mpos.Y < win_y + win_h - 1 &&
                        mpos.X > win_x && mpos.X < win_x + win_w - 1) {
                        int click_r = scroll_row + (mpos.Y - win_y - 1);
                        int click_c = scroll_col + (mpos.X - win_x - 1);
                        cursor_row = std::max(0, std::min((int)lines.size() - 1, click_r));
                        int target_line_len = (int)lines[cursor_row].size();
                        cursor_col = std::max(0, std::min(target_line_len, click_c));
                        continue;
                    }
                }

                if (flags & MOUSE_MOVED) {
                    if (is_resizing && !maximized) {
                        int new_w = mpos.X - win_x + 1;
                        int new_h = mpos.Y - win_y + 1;
                        win_w = std::max(25, std::min(term_w - win_x, new_w));
                        win_h = std::max(8, std::min(term_h - 1 - win_y, new_h));
                        restored_w = win_w;
                        restored_h = win_h;
                    } else if (is_dragging && !maximized) {
                        win_x = std::max(0, std::min(term_w - win_w, (int)mpos.X - drag_offset_x));
                        win_y = std::max(0, std::min(term_h - 1 - win_h, (int)mpos.Y - drag_offset_y));
                        restored_x = win_x;
                        restored_y = win_y;
                    }
                }

                if (btn == 0) {
                    is_dragging = false;
                    is_resizing = false;
                }
            }

            // ---------------------------------------------------------
            // EVENTOS DE TECLADO
            // ---------------------------------------------------------
            else if (rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown) {
                WORD vk = rec.Event.KeyEvent.wVirtualKeyCode;
                DWORD ctrl_state = rec.Event.KeyEvent.dwControlKeyState;
                wchar_t wc = rec.Event.KeyEvent.uChar.UnicodeChar;

                bool is_ctrl = (ctrl_state & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
                bool is_alt  = (ctrl_state & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0;

                // 1. Popup de salvamento
                if (show_exit_dlg) {
                    if (vk == VK_ESCAPE || wc == 'c' || wc == 'C') {
                        show_exit_dlg = false;
                    } else if (wc == 's' || wc == 'S' || (vk == VK_RETURN && exit_dlg_choice == 0)) {
                        std::ofstream out(file_path, std::ios::out);
                        for (size_t i = 0; i < lines.size(); ++i) {
                            out << lines[i];
                            if (i + 1 < lines.size()) out << "\n";
                        }
                        out.close();
                        running = false;
                    } else if (wc == 'n' || wc == 'N' || (vk == VK_RETURN && exit_dlg_choice == 1)) {
                        running = false;
                    } else if (vk == VK_LEFT) {
                        exit_dlg_choice = (exit_dlg_choice + 2) % 3;
                    } else if (vk == VK_RIGHT || vk == VK_TAB) {
                        exit_dlg_choice = (exit_dlg_choice + 1) % 3;
                    }
                    continue;
                }

                // 2. Modal de Erro de Compilacao
                if (show_compile_error_modal) {
                    show_compile_error_modal = false;
                    if (compile_error_line > 0 && compile_error_line <= (int)lines.size()) {
                        cursor_row = compile_error_line - 1;
                        cursor_col = 0;
                        scroll_row = std::max(0, cursor_row - inner_h / 2);
                    }
                    continue;
                }

                // 3. Modal de Sucesso de Compilacao
                if (show_compile_success_modal) {
                    show_compile_success_modal = false;
                    continue;
                }

                // 4. Modal de Ajuda
                if (show_help_modal) {
                    show_help_modal = false;
                    continue;
                }

                // 5. Modal de Opcoes do Compilador (F7 / Ctrl+T)
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
                    continue;
                }

                // 6. Modal de Caracteres Graficos (F4 / Ctrl+G)
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
                    continue;
                }

                // Atalhos Globais:
                // F1: Ajuda
                if (vk == VK_F1) {
                    show_help_modal = true;
                    continue;
                }

                // F2 / Ctrl+S: Salvar ASCII .BAS
                if (vk == VK_F2 || (is_ctrl && (vk == 'S' || vk == 's'))) {
                    std::ofstream out(file_path, std::ios::out);
                    for (size_t i = 0; i < lines.size(); ++i) {
                        out << lines[i];
                        if (i + 1 < lines.size()) out << "\n";
                    }
                    out.close();
                    modified = false;
                    status_msg = "[ Salvo com Sucesso! ]";
                    status_msg_time = std::chrono::steady_clock::now();
                    continue;
                }

                // F4 / Ctrl+G: Seletor de Caracteres Graficos ZX-81
                if (vk == VK_F4 || (is_ctrl && (vk == 'G' || vk == 'g'))) {
                    show_graphics_modal = true;
                    continue;
                }

                // F5: Maximizar / Restaurar janela
                if (vk == VK_F5) {
                    maximized = !maximized;
                    if (!maximized) {
                        win_x = restored_x;
                        win_y = restored_y;
                        win_w = restored_w;
                        win_h = restored_h;
                    }
                    continue;
                }

                // F7 / Ctrl+T: Parametros do Compilador ZXText2P
                if (vk == VK_F7 || (is_ctrl && (vk == 'T' || vk == 't'))) {
                    show_compiler_opts_modal = true;
                    compiler_opts_focus = 0;
                    continue;
                }

                // F9 / Ctrl+B: Compilar para Formato .P do ZX-81
                if (vk == VK_F9 || (is_ctrl && (vk == 'B' || vk == 'b'))) {
                    // Auto-salva arquivo fonte primeiro
                    {
                        std::ofstream out(file_path, std::ios::out);
                        for (size_t i = 0; i < lines.size(); ++i) {
                            out << lines[i];
                            if (i + 1 < lines.size()) out << "\n";
                        }
                        out.close();
                        modified = false;
                    }

                    // Nome do arquivo .P
                    std::string p_filename = file_path;
                    size_t dot_pos = p_filename.find_last_of('.');
                    if (dot_pos != std::string::npos) {
                        p_filename = p_filename.substr(0, dot_pos) + ".p";
                    } else {
                        p_filename += ".p";
                    }

                    // Juntar todas as linhas
                    std::string full_src = "";
                    for (size_t i = 0; i < lines.size(); ++i) {
                        full_src += lines[i] + "\n";
                    }

                    compiler::CompilerOptions c_opts;
                    c_opts.use_labels = cfg.get_compiler_labels_mode();
                    c_opts.start_line = cfg.get_compiler_start_line();
                    c_opts.line_increment = cfg.get_compiler_line_increment();

                    auto res = compiler::ZXText2P::compile_source(full_src, c_opts);
                    if (res.success) {
                        compiler::ZXText2P::save_p_file(p_filename, res.p_data);
                        compile_success_msg = extract_file_name(p_filename) + " (" + 
                                              std::to_string(res.total_p_bytes) + " bytes, " + 
                                              std::to_string(res.total_lines) + " linhas)";
                        show_compile_success_modal = true;
                        status_msg = "[ Compilado com Sucesso -> " + extract_file_name(p_filename) + " ]";
                        status_msg_time = std::chrono::steady_clock::now();
                    } else {
                        compile_error_msg = res.error_message;
                        compile_error_line = res.error_line;
                        show_compile_error_modal = true;
                    }
                    continue;
                }

                // ESC ou Alt+X ou Ctrl+Q: Sair
                if (vk == VK_ESCAPE || (is_alt && (vk == 'X' || vk == 'x')) || (is_ctrl && (vk == 'Q' || vk == 'q'))) {
                    if (modified) {
                        show_exit_dlg = true;
                        exit_dlg_choice = 0;
                    } else {
                        running = false;
                    }
                    continue;
                }

                // Navegacao
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
                // Backspace
                else if (vk == VK_BACK) {
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
                // Delete
                else if (vk == VK_DELETE) {
                    if (cursor_col < (int)lines[cursor_row].size()) {
                        lines[cursor_row].erase(cursor_col, 1);
                        modified = true;
                    } else if (cursor_row + 1 < (int)lines.size()) {
                        lines[cursor_row] += lines[cursor_row + 1];
                        lines.erase(lines.begin() + cursor_row + 1);
                        modified = true;
                    }
                }
                // Enter
                else if (vk == VK_RETURN) {
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
                    lines[cursor_row].insert(cursor_col, "    ");
                    cursor_col += 4;
                    modified = true;
                }
                // Caractere digitavel normal
                else if (wc >= 32 && wc != 127) {
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
        SetConsoleMode(hOut, prev_out_mode);
    }
    if (hIn != INVALID_HANDLE_VALUE) {
        SetConsoleMode(hIn, prev_in_mode);
    }
#endif

    std::cout << Colors::RESET << Colors::SHOW_CURSOR;
    std::cout << "\x1b[2J" << Colors::HOME << std::flush;

    return 0;
}

} // namespace editor
} // namespace zx81ide
