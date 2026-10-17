#include "editor/text_editor.h"
#include "core/version.h"
#include "core/config_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <thread>

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

    // Dialogo Salvar / Mensagens
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
                // Remover \r de arquivos Windows
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                lines.push_back(line);
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

    // Nome curto do arquivo para a barra de titulo
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

    // Se o terminal for menor que 82x27, ajustar para caber
    if (win_w > term_w) win_w = term_w;
    if (win_h > term_h - 1) win_h = term_h - 1; // Deixar linha inferior livre para o desktop/status

    // Centralizar janela ou posicionar no topo/esquerda
    int win_x = (term_w > win_w) ? (term_w - win_w) / 2 : 0;
    int win_y = (term_h - 1 > win_h) ? (term_h - 1 - win_h) / 2 : 0;

    // Guardar tamanho restaurado
    int restored_x = win_x;
    int restored_y = win_y;
    int restored_w = win_w;
    int restored_h = win_h;

    // Estado de clique e arrasto do mouse
    bool is_dragging = false;
    int drag_offset_x = 0;
    int drag_offset_y = 0;

    bool is_resizing = false;

    // Dialogo modal de confirmacao de saida
    bool show_exit_dlg = false;
    int exit_dlg_choice = 0; // 0 = Sim, 1 = Nao, 2 = Cancelar

    // Obter tema atual
    auto& cfg = core::ConfigManager::instance();
    std::string theme_name = cfg.get_theme();
    EditorTheme theme = get_editor_theme(theme_name);

    std::cout << Colors::HIDE_CURSOR;

    // Loop Principal de Edicao
    while (running) {
        get_terminal_size(term_w, term_h);

        // Ajustar dimensoes se maximizado
        if (maximized) {
            win_x = 0;
            win_y = 0;
            win_w = term_w;
            win_h = term_h - 1;
        } else {
            // Clampar para os limites do terminal
            if (win_w > term_w) win_w = term_w;
            if (win_h > term_h - 1) win_h = term_h - 1;
            if (win_x + win_w > term_w) win_x = std::max(0, term_w - win_w);
            if (win_y + win_h > term_h - 1) win_y = std::max(0, term_h - 1 - win_h);
        }

        int inner_w = std::max(1, win_w - 2);
        int inner_h = std::max(1, win_h - 2);

        // Ajustar cursor dentro dos limites do texto
        cursor_row = std::max(0, std::min((int)lines.size() - 1, cursor_row));
        int cur_line_len = (int)lines[cursor_row].size();
        cursor_col = std::max(0, std::min(cur_line_len, cursor_col));

        // Ajustar rolagem da janela para manter cursor visivel
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

        // Calcular posicoes dos indicadores das barras de rolagem
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
            // Caso fora da janela: desenhar desktop
            if (y < win_y || y >= win_y + win_h) {
                buf << theme.desktop_fg << theme.desktop_bg;
                for (int x = 0; x < term_w; ++x) {
                    buf << theme.desktop_char;
                }
                buf << Colors::RESET << "\n";
                continue;
            }

            // Fundo desktop a esquerda da janela
            if (win_x > 0) {
                buf << theme.desktop_fg << theme.desktop_bg;
                for (int x = 0; x < win_x; ++x) {
                    buf << theme.desktop_char;
                }
            }

            // LINHA 0: BARRA DE TITULO / BORDA SUPERIOR
            if (y == win_y) {
                buf << theme.frame_fg << theme.frame_bg;
                buf << "╔"; // Canto superior esquerdo

                // Botao fechar +=*= (quadrado pequeno)
                buf << "═[";
                buf << theme.btn_fg << theme.btn_bg << "■";
                buf << theme.frame_fg << theme.frame_bg << "]═";

                // Titulo com o nome do arquivo
                std::string title_str = " " + display_name + (modified ? " *" : "") + " ";
                int available_title_space = win_w - 14; // Reserva espaco para close, maximize e bordas
                if ((int)title_str.size() > available_title_space && available_title_space > 0) {
                    title_str = title_str.substr(0, available_title_space);
                }

                // Linha dupla antes e depois do titulo
                int remaining_bar = win_w - 14 - (int)title_str.size();
                if (remaining_bar < 0) remaining_bar = 0;
                int left_bar = remaining_bar / 2;
                int right_bar = remaining_bar - left_bar;

                for (int i = 0; i < left_bar; ++i) buf << "═";
                buf << theme.title_fg << theme.title_bg << title_str;
                buf << theme.frame_fg << theme.frame_bg;
                for (int i = 0; i < right_bar; ++i) buf << "═";

                // Botao maximizar/restaurar (=^=| ou =v=)
                buf << "═[";
                buf << theme.btn_fg << theme.btn_bg;
                if (maximized) {
                    buf << "↕"; // Seta dupla indicando restauracao
                } else {
                    buf << "▲"; // Seta para cima indicando maximizacao
                }
                buf << theme.frame_fg << theme.frame_bg << "]═";

                buf << "╗"; // Canto superior direito
            }
            // LINHAS 1 A H-2: INTERIOR DO EDITOR E BARRA VERTICAL
            else if (y > win_y && y < win_y + win_h - 1) {
                int vr = y - win_y - 1; // Indice da linha visivel no editor
                int doc_line_idx = scroll_row + vr;

                buf << theme.frame_fg << theme.frame_bg << "║";

                // Conteudo da linha de texto
                buf << theme.text_fg << theme.text_bg;
                std::string line_content = "";
                if (doc_line_idx < (int)lines.size()) {
                    std::string raw_line = lines[doc_line_idx];
                    if (scroll_col < (int)raw_line.size()) {
                        line_content = raw_line.substr(scroll_col);
                    }
                }
                if ((int)line_content.size() > inner_w) {
                    line_content = line_content.substr(0, inner_w);
                }
                buf << line_content;
                int pad_spaces = inner_w - (int)line_content.size();
                if (pad_spaces > 0) {
                    buf << std::string(pad_spaces, ' ');
                }

                // Barra de rolagem vertical na borda direita
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

                // Seta esquerda da barra de rolagem horizontal
                buf << theme.scroll_arrow_fg << theme.scroll_arrow_bg << "◄";

                // Trilho horizontal com indicador thumb
                for (int i = 0; i < h_track_len; ++i) {
                    if (i == h_thumb_pos) {
                        buf << theme.scroll_thumb_fg << theme.scroll_thumb_bg << "█";
                    } else {
                        buf << theme.scroll_track_fg << theme.scroll_track_bg << "░";
                    }
                }

                // Seta direita da barra de rolagem horizontal
                buf << theme.scroll_arrow_fg << theme.scroll_arrow_bg << "►";

                // Espaco da moldura inferior com indicador de posicao
                int remaining_bottom = win_w - 4 - h_track_len;
                std::string pos_tag = " " + std::to_string(cursor_row + 1) + ":" + std::to_string(cursor_col + 1) + " ";
                if ((int)pos_tag.size() <= remaining_bottom) {
                    int pad = remaining_bottom - (int)pos_tag.size();
                    for (int i = 0; i < pad; ++i) buf << theme.frame_fg << theme.frame_bg << "═";
                    buf << theme.frame_fg << theme.frame_bg << pos_tag;
                } else {
                    for (int i = 0; i < remaining_bottom; ++i) buf << theme.frame_fg << theme.frame_bg << "═";
                }

                // Juncao das barras de rolagem: caractere + de conexao de quadro duplo (╬)
                buf << theme.frame_fg << theme.frame_bg << "╬";
            }

            // Fundo desktop a direita da janela
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
            std::string help_bar = " F1:Ajuda  F2:Salvar  F5:Zoom  Esc:Sair  | L:" + 
                                   std::to_string(cursor_row + 1) + " C:" + std::to_string(cursor_col + 1);

            // Se houver mensagem temporaria recente (ultimos 3 segundos)
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

        // SE O DIALOGO DE CONFIRMACAO DE SAIDA ESTIVER ATIVO
        if (show_exit_dlg) {
            int dlg_w = 46;
            int dlg_h = 7;
            int dlg_x = std::max(0, (term_w - dlg_w) / 2);
            int dlg_y = std::max(0, (term_h - dlg_h) / 2);

            // Mover cursor ANSI para desenhar o popup
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
            // Botao Sim
            if (exit_dlg_choice == 0) buf << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg << "[ Sim (S) ]";
            else buf << theme.dlg_btn_fg << theme.dlg_btn_bg << "[ Sim (S) ]";
            buf << theme.dlg_body_bg << "  ";
            // Botao Nao
            if (exit_dlg_choice == 1) buf << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg << "[ Nao (N) ]";
            else buf << theme.dlg_btn_fg << theme.dlg_btn_bg << "[ Nao (N) ]";
            buf << theme.dlg_body_bg << "  ";
            // Botao Cancelar
            if (exit_dlg_choice == 2) buf << theme.dlg_btn_sel_fg << theme.dlg_btn_sel_bg << "[ Cancelar (C) ]";
            else buf << theme.dlg_btn_fg << theme.dlg_btn_bg << "[ Cancelar (C) ]";
            buf << theme.dlg_body_bg << "   " << theme.dlg_border_fg << theme.dlg_border_bg << "║";

            buf << "\x1b[" << (dlg_y + 6) << ";" << (dlg_x + 1) << "H";
            buf << "╚════════════════════════════════════════════╝" << Colors::RESET;
        }

        // Posicionar cursor real do console na posicao de digitacao
        if (!show_exit_dlg) {
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

                // Rolagem pelo Scroll Wheel do Mouse
                if (flags & MOUSE_WHEELED) {
                    short delta = (short)HIWORD(rec.Event.MouseEvent.dwButtonState);
                    if (delta > 0) {
                        cursor_row = std::max(0, cursor_row - 3);
                    } else if (delta < 0) {
                        cursor_row = std::min((int)lines.size() - 1, cursor_row + 3);
                    }
                    continue;
                }

                // Clique do botao esquerdo
                if (btn & FROM_LEFT_1ST_BUTTON_PRESSED) {
                    if (show_exit_dlg) {
                        int dlg_w = 46;
                        int dlg_h = 7;
                        int dlg_x = std::max(0, (term_w - dlg_w) / 2);
                        int dlg_y = std::max(0, (term_h - dlg_h) / 2);

                        if (mpos.Y == dlg_y + 4) {
                            // Clicou na linha dos botoes
                            if (mpos.X >= dlg_x + 3 && mpos.X <= dlg_x + 13) {
                                // Salvar e sair
                                std::ofstream out(file_path, std::ios::out);
                                for (size_t i = 0; i < lines.size(); ++i) {
                                    out << lines[i];
                                    if (i + 1 < lines.size()) out << "\n";
                                }
                                out.close();
                                running = false;
                            } else if (mpos.X >= dlg_x + 16 && mpos.X <= dlg_x + 26) {
                                // Sair sem salvar
                                running = false;
                            } else if (mpos.X >= dlg_x + 29 && mpos.X <= dlg_x + 43) {
                                // Cancelar
                                show_exit_dlg = false;
                            }
                        }
                        continue;
                    }

                    // 1. Botao fechar [■] (Colunas win_x + 2 .. win_x + 4, Linha win_y)
                    if (mpos.Y == win_y && mpos.X >= win_x + 2 && mpos.X <= win_x + 4) {
                        if (modified) {
                            show_exit_dlg = true;
                            exit_dlg_choice = 0;
                        } else {
                            running = false;
                        }
                        continue;
                    }

                    // 2. Botao maximizar/restaurar (Colunas win_w - 4 .. win_w - 2)
                    if (mpos.Y == win_y && mpos.X >= win_x + win_w - 4 && mpos.X <= win_x + win_w - 2) {
                        maximized = !maximized;
                        if (!maximized) {
                            win_x = restored_x;
                            win_y = restored_y;
                            win_w = restored_w;
                            win_h = restored_h;
                        }
                        continue;
                    }

                    // 3. Juncao inferior direita (╬) para redimensionamento
                    if (mpos.Y == win_y + win_h - 1 && mpos.X == win_x + win_w - 1) {
                        if (!maximized) {
                            is_resizing = true;
                        }
                        continue;
                    }

                    // 4. Barra de rolagem vertical (coluna win_x + win_w - 1)
                    if (mpos.X == win_x + win_w - 1) {
                        if (mpos.Y == win_y + 1) {
                            // Seta para cima ▲
                            cursor_row = std::max(0, cursor_row - 1);
                        } else if (mpos.Y == win_y + win_h - 2) {
                            // Seta para baixo ▼
                            cursor_row = std::min((int)lines.size() - 1, cursor_row + 1);
                        } else if (mpos.Y > win_y + 1 && mpos.Y < win_y + win_h - 2) {
                            // Trilho vertical
                            int track_click = mpos.Y - (win_y + 2);
                            if (v_track_len > 0) {
                                float ratio = (float)track_click / (float)v_track_len;
                                cursor_row = (int)(ratio * (total_lines - 1));
                            }
                        }
                        continue;
                    }

                    // 5. Barra de rolagem horizontal (linha win_y + win_h - 1)
                    if (mpos.Y == win_y + win_h - 1) {
                        if (mpos.X == win_x + 1) {
                            // Seta esquerda ◄
                            cursor_col = std::max(0, cursor_col - 4);
                        } else if (mpos.X == win_x + win_w - 7) {
                            // Seta direita ►
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

                    // 6. Arrastar janela pela barra de titulo
                    if (mpos.Y == win_y && !maximized) {
                        is_dragging = true;
                        drag_offset_x = mpos.X - win_x;
                        drag_offset_y = mpos.Y - win_y;
                        continue;
                    }

                    // 7. Clique na area util de texto
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

                // Movimento do mouse com arrasto ativo
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

                // Soltura de botao do mouse
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

                // Se o popup de salvamento estiver aberto
                if (show_exit_dlg) {
                    if (vk == VK_ESCAPE || wc == 'c' || wc == 'C') {
                        show_exit_dlg = false;
                    } else if (wc == 's' || wc == 'S' || (vk == VK_RETURN && exit_dlg_choice == 0)) {
                        // Salvar e sair
                        std::ofstream out(file_path, std::ios::out);
                        for (size_t i = 0; i < lines.size(); ++i) {
                            out << lines[i];
                            if (i + 1 < lines.size()) out << "\n";
                        }
                        out.close();
                        running = false;
                    } else if (wc == 'n' || wc == 'N' || (vk == VK_RETURN && exit_dlg_choice == 1)) {
                        // Sair sem salvar
                        running = false;
                    } else if (vk == VK_LEFT) {
                        exit_dlg_choice = (exit_dlg_choice + 2) % 3;
                    } else if (vk == VK_RIGHT || vk == VK_TAB) {
                        exit_dlg_choice = (exit_dlg_choice + 1) % 3;
                    }
                    continue;
                }

                // Atalhos Globais: F2 / Ctrl+S (Salvar)
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

                // Teclas de Navegacao
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
                } else if (vk == VK_PRIOR) { // Page Up
                    cursor_row = std::max(0, cursor_row - inner_h);
                } else if (vk == VK_NEXT) { // Page Down
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
                    // Converter wchar para UTF-8 basico
                    char c = (char)wc;
                    lines[cursor_row].insert(lines[cursor_row].begin() + cursor_col, c);
                    cursor_col++;
                    modified = true;
                }
            }
        }
#else
        // Fallback para ambientes sem Windows console
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
