#include "compiler/zxtext2p.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <cctype>
#include <algorithm>
#include <map>

namespace zx81ide {
namespace compiler {

namespace {

// Tabela de tokens do Sinclair ZX-81 (códigos de 255 descendo até 189)
struct TokenPair {
    const char* name;
    const char* alias;
    uint8_t code;
};

const TokenPair TOKEN_TABLE[] = {
    { "copy", "", 255 },
    { "return", "", 254 },
    { "clear", "", 253 },
    { "unplot", "", 252 },
    { "cls", "", 251 },
    { "if", "", 250 },
    { "randomize", "rand", 249 },
    { "save", "", 248 },
    { "run", "", 247 },
    { "plot", "", 246 },
    { "print", "", 245 },
    { "poke", "", 244 },
    { "next", "", 243 },
    { "pause", "", 242 },
    { "let", "", 241 },
    { "list", "", 240 },
    { "load", "", 239 },
    { "input", "", 238 },
    { "go sub", "gosub", 237 },
    { "go to", "goto", 236 },
    { "for", "", 235 },
    { "rem", "", 234 },
    { "dim", "", 233 },
    { "continue", "cont", 232 },
    { "scroll", "", 231 },
    { "new", "", 230 },
    { "fast", "", 229 },
    { "slow", "", 228 },
    { "stop", "", 227 },
    { "llist", "", 226 },
    { "lprint", "", 225 },
    { "step", "", 224 },
    { "to", "", 223 },
    { "then", "", 222 },
    { "<>", "", 221 },
    { ">=", "", 220 },
    { "<=", "", 219 },
    { "and", "", 218 },
    { "or", "", 217 },
    { "**", "", 216 },
    { "not", "", 215 },
    { "chr$", "", 214 },
    { "str$", "", 213 },
    { "usr", "", 212 },
    { "peek", "", 211 },
    { "abs", "", 210 },
    { "sgn", "", 209 },
    { "sqr", "", 208 },
    { "int", "", 207 },
    { "exp", "", 206 },
    { "ln", "", 205 },
    { "atn", "", 204 },
    { "acs", "", 203 },
    { "asn", "", 202 },
    { "tan", "", 201 },
    { "cos", "", 200 },
    { "sin", "", 199 },
    { "len", "", 198 },
    { "val", "", 197 },
    { "code", "", 196 },
    { "tab", "", 194 },
    { "at", "", 193 },
    { "pi", "", 191 },     // Traduzido para caractere ZX 66
    { "inkey$", "", 190 }, // Traduzido para caractere ZX 65
    { "rnd", "", 189 }     // Traduzido para caractere ZX 64
};

const size_t NUM_TOKENS = sizeof(TOKEN_TABLE) / sizeof(TOKEN_TABLE[0]);

const char* GRAPHIC_LOOKUP[] = {
    "  ",   "' ",   " '",   "''",   ". ",   ": ",   ".'",   ":'",   "##",   ",,",   "~~",
    "::",   ".:",   ":.",   "..",   "':",   " :",   "'.",   " .",   "@@",   ";;",   "!!",
    nullptr
};

int decode_block_graphic(const char* p) {
    for (int i = 0; GRAPHIC_LOOKUP[i] != nullptr; ++i) {
        if (p[0] == GRAPHIC_LOOKUP[i][0] && p[1] == GRAPHIC_LOOKUP[i][1]) {
            if (i > 10) return i + 117; // 11..21 -> 128..138
            return i;                   // 0..10  -> 0..10
        }
    }
    return -1;
}

std::string to_lower_str(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

} // anonymous namespace

bool ZXText2P::double_to_zx81_fp(double num, int& exp, uint32_t& mantissa) {
    if (num == 0.0) {
        exp = 0;
        mantissa = 0;
        return true;
    }
    if (num < 0.0) {
        num = -num;
    }
    int e = (int)std::floor(std::log(num) / std::log(2.0));
    if (e < -129 || e > 126) {
        return false;
    }
    double frac = (num / std::pow(2.0, e) - 1.0) * 2147483648.0 + 0.5;
    uint32_t m = (uint32_t)std::floor(frac);
    m = m & 0x7FFFFFFFU;
    exp = e + 129;
    mantissa = m;
    return true;
}

uint8_t ZXText2P::ascii_to_zx(char c) {
    unsigned char uc = (unsigned char)c;
    uc = (unsigned char)std::tolower(uc);

    if (uc >= 'a' && uc <= 'z') return (uint8_t)(uc - 59); // 38..63 (A..Z)
    if (uc >= '0' && uc <= '9') return (uint8_t)(uc - 20); // 28..37 (0..9)

    switch (uc) {
        case ' ': return 0;
        case '"': return 11;
        case '#': return 12; // '#' mapeado para o símbolo Pound £ do ZX81
        case '$': return 13;
        case ':': return 14;
        case '?': return 15;
        case '(': return 16;
        case ')': return 17;
        case '>': return 18;
        case '<': return 19;
        case '=': return 20;
        case '+': return 21;
        case '-': return 22;
        case '*': return 23;
        case '/': return 24;
        case ';': return 25;
        case ',': return 26;
        case '.': return 27;

        // Placeholders de funções especiais traduzidos para códigos de caracteres do ZX-81
        case 191: return 66; // PI
        case 190: return 65; // INKEY$
        case 189: return 64; // RND

        default:
            return uc; // Tokens e caracteres já mapeados permanecem intactos
    }
}

bool ZXText2P::is_keyword(const std::string& word) {
    std::string lower = to_lower_str(word);
    for (size_t i = 0; i < NUM_TOKENS; ++i) {
        if (lower == TOKEN_TABLE[i].name) return true;
        if (TOKEN_TABLE[i].alias[0] != '\0' && lower == TOKEN_TABLE[i].alias) return true;
    }
    return false;
}

const std::vector<std::string>& ZXText2P::get_all_keywords() {
    static std::vector<std::string> kw;
    if (kw.empty()) {
        for (size_t i = 0; i < NUM_TOKENS; ++i) {
            std::string name = TOKEN_TABLE[i].name;
            std::transform(name.begin(), name.end(), name.begin(), ::toupper);
            kw.push_back(name);
            if (TOKEN_TABLE[i].alias[0] != '\0') {
                std::string alias = TOKEN_TABLE[i].alias;
                std::transform(alias.begin(), alias.end(), alias.begin(), ::toupper);
                kw.push_back(alias);
            }
        }
    }
    return kw;
}

const std::vector<GraphicCharDef>& ZXText2P::get_graphics_table() {
    static std::vector<GraphicCharDef> table = {
        { 0,   "\\  ", " ",  "Espaco em Branco (Vazio)" },
        { 1,   "\\' ", "▘", "Quadrante Superior Esquerdo" },
        { 2,   "\\ '", "▝", "Quadrante Superior Direito" },
        { 3,   "\\''", "▀", "Metade Superior (Sup. Esq. + Sup. Dir.)" },
        { 4,   "\\. ", "▖", "Quadrante Inferior Esquerdo" },
        { 5,   "\\: ", "▌", "Lado Esquerdo Completo (Sup. + Inf. Esq.)" },
        { 6,   "\\.'", "▞", "Diagonal (Inf. Esq. + Sup. Dir.)" },
        { 7,   "\\:'", "▛", "3 Quadrantes (exceto Inf. Direito)" },
        { 8,   "\\##", "▒", "Xadrez / Checkerboard Cinza 1" },
        { 9,   "\\,,", "░", "Hachurado Cinza 2" },
        { 10,  "\\~~", "▓", "Hachurado Cinza 3" },
        { 128, "\\::", "█", "Bloco Solido Preto (Inverso de Espaco)" },
        { 129, "\\.:", "▟", "Inverso do Sup. Esquerdo (3 Quadrantes Pretos)" },
        { 130, "\\:.", "▙", "Inverso do Sup. Direito (3 Quadrantes Pretos)" },
        { 131, "\\..", "▄", "Metade Inferior (Inf. Esq. + Inf. Dir.)" },
        { 132, "\\':", "▜", "Inverso do Inf. Esquerdo (3 Quadrantes Pretos)" },
        { 133, "\\ :", "▐", "Lado Direito Completo (Sup. + Inf. Dir.)" },
        { 134, "\\'.", "▚", "Diagonal Inversa (Sup. Esq. + Inf. Dir.)" },
        { 135, "\\ .", "▗", "Apenas Quadrante Inferior Direito" },
        { 136, "\\@@", "▒", "Inverso do Xadrez / Checkerboard 1" },
        { 137, "\\;;", "░", "Inverso do Hachurado 2" },
        { 138, "\\!!", "▓", "Inverso do Hachurado 3" },
        { 192, "\\\"",  "\"", "Aspas Duplas Escapadas dentro de String" },
        { 12,  "\\#",  "£", "Simbolo Libra Esterlina (£ do ZX81)" }
    };
    return table;
}

CompilerResult ZXText2P::compile_source(const std::string& source_code, const CompilerOptions& options) {
    CompilerResult result;
    result.success = false;

    // 1. Dividir em linhas fisicas com suporte a quebra de linha por barra invertida '\'
    std::vector<std::pair<int, std::string>> raw_lines; // pair<num_linha_original, texto>
    std::istringstream stream(source_code);
    std::string line;
    int current_source_line = 0;

    std::string accumulated = "";
    int accum_start_line = 1;

    while (std::getline(stream, line)) {
        current_source_line++;
        // Remove \r final se presente
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (accumulated.empty()) {
            accum_start_line = current_source_line;
        }

        // Verifica se a linha termina com '\' indicando continuacao
        if (!line.empty() && line.back() == '\\') {
            accumulated += line.substr(0, line.size() - 1);
            continue;
        }

        accumulated += line;

        // Linhas em branco ou comentarios '#' sao ignorados na saida binaria
        std::string trimmed = accumulated;
        size_t first_non_space = trimmed.find_first_not_of(" \t");
        if (first_non_space == std::string::npos || trimmed[first_non_space] == '#') {
            accumulated = "";
            continue;
        }

        raw_lines.push_back({ accum_start_line, accumulated });
        accumulated = "";
    }

    if (!accumulated.empty()) {
        std::string trimmed = accumulated;
        size_t first_non_space = trimmed.find_first_not_of(" \t");
        if (first_non_space != std::string::npos && trimmed[first_non_space] != '#') {
            raw_lines.push_back({ accum_start_line, accumulated });
        }
    }

    if (raw_lines.empty()) {
        // Gera arquivo .P Sinclair ZX-81 valido vazio (sem linhas de programa)
        std::vector<uint8_t> p_file(116, 0);
        uint16_t d_file_addr = 0x4009 + 116; // 0x407D
        p_file[3] = (uint8_t)(d_file_addr & 0xFF);
        p_file[4] = (uint8_t)(d_file_addr >> 8);
        uint16_t df_cc_addr = d_file_addr + 1;
        p_file[5] = (uint8_t)(df_cc_addr & 0xFF);
        p_file[6] = (uint8_t)(df_cc_addr >> 8);
        uint16_t vars_addr = d_file_addr + 793;
        p_file[7] = (uint8_t)(vars_addr & 0xFF);
        p_file[8] = (uint8_t)(vars_addr >> 8);
        uint16_t e_line_addr = vars_addr + 1;
        p_file[11] = (uint8_t)(e_line_addr & 0xFF);
        p_file[12] = (uint8_t)(e_line_addr >> 8);
        uint16_t ch_add_addr = e_line_addr + 1;
        p_file[13] = (uint8_t)(ch_add_addr & 0xFF);
        p_file[14] = (uint8_t)(ch_add_addr >> 8);
        uint16_t stk_addr = ch_add_addr + 1;
        p_file[17] = p_file[19] = (uint8_t)(stk_addr & 0xFF);
        p_file[18] = p_file[20] = (uint8_t)(stk_addr >> 8);
        p_file[22] = 0x5D;
        p_file[23] = 0x40;
        p_file[25] = 2;
        p_file[28] = p_file[29] = 255;
        p_file[30] = 0x0F;
        p_file[31] = 55;
        p_file[32] = p_file[3];
        p_file[33] = p_file[4];
        p_file[39] = 0x6B;
        p_file[40] = 0x0C;
        p_file[43] = 0x7D;
        p_file[44] = 0xFD;
        p_file[47] = 0xBC;
        p_file[48] = 0x21;
        p_file[49] = 0x18;
        p_file[50] = 0x40;
        p_file[83] = 118;

        // D_FILE: 1 byte 0x76, seguido de 24 linhas de (32 espacos 0x00 + 1 byte 0x76)
        p_file.push_back(0x76);
        for (int row = 0; row < 24; ++row) {
            for (int col = 0; col < 32; ++col) {
                p_file.push_back(0x00);
            }
            p_file.push_back(0x76);
        }
        // VARS: 0x80
        p_file.push_back(0x80);

        result.success = true;
        result.total_lines = 0;
        result.program_bytes = 0;
        result.p_data = p_file;
        result.total_p_bytes = p_file.size();
        return result;
    }

    // 2. Modo Labels vs Modo Numerado (com auto-deteccao)
    bool use_labels = options.use_labels;
    if (options.auto_detect_mode && !use_labels) {
        for (size_t i = 0; i < raw_lines.size(); ++i) {
            const std::string& text = raw_lines[i].second;
            size_t p = text.find_first_not_of(" \t");
            if (p != std::string::npos) {
                if (text[p] == '@' || !std::isdigit((unsigned char)text[p])) {
                    use_labels = true;
                    break;
                }
            }
        }
    }

    std::map<std::string, int> label_map;
    std::vector<int> assigned_line_numbers;
    assigned_line_numbers.resize(raw_lines.size(), 0);

    int first_line_num = -1;

    if (use_labels) {
        // Pass 1: Associar linhas e registrar labels
        int current_auto_line = options.start_line;
        for (size_t i = 0; i < raw_lines.size(); ++i) {
            int src_line = raw_lines[i].first;
            std::string& text = raw_lines[i].second;

            // Trim leading
            size_t p = text.find_first_not_of(" \t");
            if (p == std::string::npos) p = 0;
            std::string statement = text.substr(p);

            // Se a linha ja comeca com numero explicito (ex: 10 PRINT ...), respeita o numero!
            if (!statement.empty() && std::isdigit((unsigned char)statement[0])) {
                size_t end_num = 0;
                while (end_num < statement.size() && std::isdigit((unsigned char)statement[end_num])) {
                    end_num++;
                }
                int explicit_line = std::stoi(statement.substr(0, end_num));
                assigned_line_numbers[i] = explicit_line;
                current_auto_line = explicit_line + options.line_increment;
                if (first_line_num == -1) first_line_num = explicit_line;

                size_t after_num = statement.find_first_not_of(" \t", end_num);
                if (after_num != std::string::npos) {
                    text = statement.substr(after_num);
                } else {
                    text = "";
                }
                continue;
            }

            // Se for definicao de label: @minhalabel:
            if (!statement.empty() && statement[0] == '@') {
                size_t colon = statement.find(':');
                if (colon == std::string::npos) {
                    result.error_message = "Definicao incompleta de label (faltou ':' ao final do nome)";
                    result.error_line = src_line;
                    return result;
                }
                std::string label_name = statement.substr(1, colon - 1);
                if (label_name.size() > 32) {
                    result.error_message = "Nome de label muito longo (maximo de 32 caracteres): @" + label_name;
                    result.error_line = src_line;
                    return result;
                }
                if (label_map.find(label_name) != label_map.end()) {
                    result.error_message = "Redefinicao de label duplicada: @" + label_name;
                    result.error_line = src_line;
                    return result;
                }

                label_map[label_name] = current_auto_line;

                // Resto da linha apos o ':'
                size_t after_colon = statement.find_first_not_of(" \t", colon + 1);
                if (after_colon == std::string::npos) {
                    // Linha continha apenas a label, atualiza o texto para vazio
                    text = "";
                } else {
                    text = statement.substr(after_colon);
                }
            }

            if (current_auto_line > 9999) {
                result.error_message = "Numero de linha gerado ultrapassou o limite do ZX-81 (9999). Ajuste -s ou -i.";
                result.error_line = src_line;
                return result;
            }

            assigned_line_numbers[i] = current_auto_line;
            if (first_line_num == -1 && !text.empty()) {
                first_line_num = current_auto_line;
            }

            // Se a linha ficou vazia (apenas declarava a label), a proxima linha tera o mesmo numero
            if (!text.empty()) {
                current_auto_line += options.line_increment;
            }
        }
    } else {
        // Modo com numeracao explicita
        int last_line_num = -1;
        for (size_t i = 0; i < raw_lines.size(); ++i) {
            int src_line = raw_lines[i].first;
            std::string& text = raw_lines[i].second;

            size_t p = text.find_first_not_of(" \t");
            if (p == std::string::npos || !std::isdigit((unsigned char)text[p])) {
                result.error_message = "Linha sem numeracao detectada no modo padrao. Use o modo -l ou inclua o numero da linha.";
                result.error_line = src_line;
                return result;
            }

            size_t end_num = p;
            while (end_num < text.size() && std::isdigit((unsigned char)text[end_num])) {
                end_num++;
            }

            int line_num = std::stoi(text.substr(p, end_num - p));
            if (line_num <= last_line_num) {
                result.error_message = "Numero da linha (" + std::to_string(line_num) + 
                                       ") nao e maior que a linha anterior (" + std::to_string(last_line_num) + ")";
                result.error_line = src_line;
                return result;
            }
            if (line_num > 9999) {
                result.error_message = "Numero de linha fora dos limites do ZX-81 (maximo 9999): " + std::to_string(line_num);
                result.error_line = src_line;
                return result;
            }

            assigned_line_numbers[i] = line_num;
            last_line_num = line_num;
            if (first_line_num == -1) first_line_num = line_num;

            // Corta o numero da linha
            size_t after_num = text.find_first_not_of(" \t", end_num);
            if (after_num != std::string::npos) {
                text = text.substr(after_num);
            } else {
                text = "";
            }
        }
    }

    if (first_line_num == -1) first_line_num = 10;

    // 3. Processar e Tokenizar cada linha
    std::vector<uint8_t> program_bytes;

    for (size_t i = 0; i < raw_lines.size(); ++i) {
        int src_line = raw_lines[i].first;
        std::string line_text = raw_lines[i].second;
        int line_num = assigned_line_numbers[i];

        // Se a linha ficou vazia (ex: apenas continha @label: isolada), nao gera instrucao
        if (line_text.empty()) {
            continue;
        }

        // Substituicao de labels @nome -> 100
        if (use_labels) {
            size_t at_pos = 0;
            while ((at_pos = line_text.find('@', at_pos)) != std::string::npos) {
                if (at_pos > 0 && line_text[at_pos - 1] == '\\') {
                    // Escaped \@ -> ignora
                    at_pos++;
                    continue;
                }

                // Extrair nome da label
                size_t end_label = at_pos + 1;
                while (end_label < line_text.size() && 
                       (std::isalnum((unsigned char)line_text[end_label]) || line_text[end_label] == '_')) {
                    end_label++;
                }

                std::string target_label = line_text.substr(at_pos + 1, end_label - at_pos - 1);
                auto it = label_map.find(target_label);
                if (it == label_map.end()) {
                    result.error_message = "Label indefinida no programa: @" + target_label;
                    result.error_line = src_line;
                    return result;
                }

                std::string num_str = std::to_string(it->second);
                line_text.replace(at_pos, end_label - at_pos, num_str);
                at_pos += num_str.size();
            }
        }

        // Mascara strings e detecta REM
        std::string lcase_masked = "";
        bool in_quotes = false;
        for (char c : line_text) {
            if (c == '"') in_quotes = !in_quotes;
            if (in_quotes && c != '"') {
                lcase_masked += ' ';
            } else {
                lcase_masked += (char)std::tolower((unsigned char)c);
            }
        }

        // Verifica REM
        size_t rem_idx = std::string::npos;
        for (size_t p = 0; p + 3 <= lcase_masked.size(); ++p) {
            if (lcase_masked.substr(p, 3) == "rem") {
                bool left_ok = (p == 0 || !std::isalpha((unsigned char)lcase_masked[p - 1]));
                bool right_ok = (p + 3 == lcase_masked.size() || !std::isalpha((unsigned char)lcase_masked[p + 3]));
                if (left_ok && right_ok) {
                    rem_idx = p;
                    break;
                }
            }
        }

        std::string tokenized_buf = line_text;
        size_t limit = (rem_idx != std::string::npos ? rem_idx : tokenized_buf.size());

        // Tokenizacao de palavras-chave
        for (size_t t = 0; t < NUM_TOKENS; ++t) {
            const char* words[2] = { TOKEN_TABLE[t].name, TOKEN_TABLE[t].alias };
            uint8_t tok_code = TOKEN_TABLE[t].code;

            for (int w = 0; w < 2; ++w) {
                if (words[w][0] == '\0') continue;
                std::string kw = words[w];
                size_t kw_len = kw.size();

                size_t search_pos = 0;
                while (search_pos + kw_len <= limit) {
                    size_t found = lcase_masked.find(kw, search_pos);
                    if (found == std::string::npos || found + kw_len > limit) break;

                    bool is_symbol_op = (kw == "<>" || kw == "<=" || kw == ">=" || kw == "**");
                    bool left_ok = (found == 0 || !std::isalpha((unsigned char)lcase_masked[found - 1]));
                    bool right_ok = (found + kw_len == lcase_masked.size() || !std::isalpha((unsigned char)lcase_masked[found + kw_len]));

                    if (is_symbol_op || (left_ok && right_ok)) {
                        tokenized_buf[found] = (char)tok_code;
                        lcase_masked[found] = (char)tok_code;
                        for (size_t k = 1; k < kw_len; ++k) {
                            tokenized_buf[found + k] = 1; // Marcador para descarte
                            lcase_masked[found + k] = 1;
                        }
                    }
                    search_pos = found + kw_len;
                }
            }
        }

        if (rem_idx != std::string::npos) {
            tokenized_buf[rem_idx] = (char)234; // REM token
            for (size_t k = 1; k < 3; ++k) {
                tokenized_buf[rem_idx + k] = 1;
            }
            if (rem_idx + 3 < tokenized_buf.size() && tokenized_buf[rem_idx + 3] == ' ') {
                tokenized_buf[rem_idx + 3] = 1;
            }
        }

        // Converte caracteres e numeros para bytes finais do ZX-81
        std::vector<uint8_t> line_bytes;
        bool in_rem_mode = false;
        in_quotes = false;

        size_t ptr = 0;
        while (ptr < tokenized_buf.size()) {
            char ch = tokenized_buf[ptr];

            if (ch == '"') in_quotes = !in_quotes;

            // Ignorar marcadores 0x01, tabs e espacos fora de strings/rem
            if (ch == 1 || ch == '\t' || (!in_quotes && !in_rem_mode && ch == ' ')) {
                ptr++;
                continue;
            }

            if ((uint8_t)ch == 234) {
                in_rem_mode = true;
                line_bytes.push_back(234);
                ptr++;
                continue;
            }

            // Video Inverso (%)
            if (!in_rem_mode && ch == '%') {
                if (ptr + 1 >= tokenized_buf.size()) {
                    result.error_message = "Caractere '%' incompleto para video inverso ao final da linha";
                    result.error_line = src_line;
                    return result;
                }
                char inv_c = tokenized_buf[ptr + 1];
                uint8_t zx_c = ascii_to_zx(inv_c);
                line_bytes.push_back(zx_c | 128);
                ptr += 2;
                continue;
            }

            // Escape '\' (bloco grafico ou aspas duplas)
            if (!in_rem_mode && ch == '\\') {
                if (ptr + 1 >= tokenized_buf.size()) {
                    result.error_message = "Sequencia de escape '\\' incompleta ao final da linha";
                    result.error_line = src_line;
                    return result;
                }
                char next_c = tokenized_buf[ptr + 1];
                if (next_c == '"') {
                    line_bytes.push_back(192); // Escaped quote
                    ptr += 2;
                    continue;
                }
                if (next_c == '#') {
                    line_bytes.push_back(12); // Pound sign
                    ptr += 2;
                    continue;
                }
                if (ptr + 2 < tokenized_buf.size()) {
                    char blk[2] = { tokenized_buf[ptr + 1], tokenized_buf[ptr + 2] };
                    int bg = decode_block_graphic(blk);
                    if (bg != -1) {
                        line_bytes.push_back((uint8_t)bg);
                        ptr += 3;
                        continue;
                    }
                }
                result.error_message = "Sequencia de escape de bloco grafico invalida: \\" + tokenized_buf.substr(ptr + 1, 2);
                result.error_line = src_line;
                return result;
            }

            // Constantes numericas inline com representacao de ponto flutuante de 5 bytes
            bool prev_alpha = (ptr > 0 && std::isalpha((unsigned char)tokenized_buf[ptr - 1]));
            bool is_num_start = false;

            if (!in_rem_mode && !in_quotes && !prev_alpha) {
                if (std::isdigit((unsigned char)ch)) {
                    is_num_start = true;
                } else if ((ch == '-' || ch == '+' || ch == '.') && ptr + 1 < tokenized_buf.size() && std::isdigit((unsigned char)tokenized_buf[ptr + 1])) {
                    is_num_start = true;
                } else if ((ch == '-' || ch == '+') && ptr + 2 < tokenized_buf.size() && tokenized_buf[ptr + 1] == '.' && std::isdigit((unsigned char)tokenized_buf[ptr + 2])) {
                    is_num_start = true;
                }
            }

            if (is_num_start) {
                const char* start_ptr = tokenized_buf.c_str() + ptr;
                char* end_ptr = nullptr;
                double val = std::strtod(start_ptr, &end_ptr);

                if (end_ptr > start_ptr) {
                    size_t num_len = end_ptr - start_ptr;
                    for (size_t n = 0; n < num_len; ++n) {
                        line_bytes.push_back(ascii_to_zx(tokenized_buf[ptr + n]));
                    }
                    ptr += num_len;

                    // Emite byte delimitador 0x7E e os 5 bytes de ponto flutuante
                    line_bytes.push_back(0x7E);
                    int fp_exp = 0;
                    uint32_t fp_man = 0;
                    if (!double_to_zx81_fp(val, fp_exp, fp_man)) {
                        result.error_message = "Numero muito grande ou expoente fora de alcance para o Sinclair ZX-81";
                        result.error_line = src_line;
                        return result;
                    }
                    line_bytes.push_back((uint8_t)fp_exp);
                    line_bytes.push_back((uint8_t)((fp_man >> 24) & 0xFF));
                    line_bytes.push_back((uint8_t)((fp_man >> 16) & 0xFF));
                    line_bytes.push_back((uint8_t)((fp_man >> 8) & 0xFF));
                    line_bytes.push_back((uint8_t)(fp_man & 0xFF));
                    continue;
                }
            }

            // Caractere normal
            line_bytes.push_back(ascii_to_zx(ch));
            ptr++;
        }

        // Delimitador de fim de linha: 0x76 (NEWLINE/HALT)
        line_bytes.push_back(0x76);

        // Header da linha ZX81:
        // 2 bytes Big Endian: Numero da Linha
        // 2 bytes Little Endian: Tamanho da Linha em bytes
        uint16_t line_len = (uint16_t)line_bytes.size();
        program_bytes.push_back((uint8_t)(line_num >> 8));
        program_bytes.push_back((uint8_t)(line_num & 0xFF));
        program_bytes.push_back((uint8_t)(line_len & 0xFF));
        program_bytes.push_back((uint8_t)(line_len >> 8));
        program_bytes.insert(program_bytes.end(), line_bytes.begin(), line_bytes.end());

        result.total_lines++;
    }

    result.program_bytes = program_bytes.size();

    // 4. Montar arquivo binario .P completo
    // 116 bytes de System Variables (0x4009 a 0x407C)
    std::vector<uint8_t> p_file;
    p_file.resize(116, 0);

    uint16_t d_file_addr = (uint16_t)(0x407D + program_bytes.size());

    // E_PPC (Linha com o cursor de edicao)
    p_file[1] = (uint8_t)(first_line_num & 0xFF);
    p_file[2] = (uint8_t)(first_line_num >> 8);

    // D_FILE (Endereco do Display File)
    p_file[3] = (uint8_t)(d_file_addr & 0xFF);
    p_file[4] = (uint8_t)(d_file_addr >> 8);

    // DF_CC (Endereco da posicao de impressao no Display File)
    uint16_t df_cc_addr = d_file_addr + 1;
    p_file[5] = (uint8_t)(df_cc_addr & 0xFF);
    p_file[6] = (uint8_t)(df_cc_addr >> 8);

    // VARS (Endereco da area de variaveis)
    uint16_t vars_addr = d_file_addr + 793;
    p_file[7] = (uint8_t)(vars_addr & 0xFF);
    p_file[8] = (uint8_t)(vars_addr >> 8);

    // E_LINE (Ponteiro para o buffer de edicao)
    uint16_t e_line_addr = vars_addr + 1;
    p_file[11] = (uint8_t)(e_line_addr & 0xFF);
    p_file[12] = (uint8_t)(e_line_addr >> 8);

    // CH_ADD (Endereco do proximo caractere a ser interpretado)
    uint16_t ch_add_addr = e_line_addr + 1;
    p_file[13] = (uint8_t)(ch_add_addr & 0xFF);
    p_file[14] = (uint8_t)(ch_add_addr >> 8);

    // STKBOT & STKEND (Pilha da calculadora)
    uint16_t stk_addr = ch_add_addr + 1;
    p_file[17] = p_file[19] = (uint8_t)(stk_addr & 0xFF);
    p_file[18] = p_file[20] = (uint8_t)(stk_addr >> 8);

    // MEM (Memoria da calculadora = MEMBOT 0x405D)
    p_file[22] = 0x5D;
    p_file[23] = 0x40;

    // DF_SZ (Linhas na parte inferior da tela)
    p_file[25] = 2;

    // LAST_K
    p_file[28] = 255;
    p_file[29] = 255;

    // DEBOUNCE
    p_file[30] = 0x0F;

    // MARGIN (Scanlines de borda = 55 PAL)
    p_file[31] = 55;

    // NXTLIN (Proxima linha do programa)
    p_file[32] = p_file[3];
    p_file[33] = p_file[4];

    // T_ADDR (0x0C6B)
    p_file[39] = 0x6B;
    p_file[40] = 0x0C;

    // FRAMES
    p_file[43] = 0x7D;
    p_file[44] = 0xFD;

    // PR_CC
    p_file[47] = 0xBC;

    // S_POSN x e y
    p_file[48] = 0x21;
    p_file[49] = 0x18;

    // CDFLAG (Modo SLOW/FAST)
    p_file[50] = 0x40;

    // PRBUF (33 bytes de buffer de impressora, o 33o e NEWLINE 0x76)
    p_file[83] = 118; // 0x76

    // Anexar programa
    p_file.insert(p_file.end(), program_bytes.begin(), program_bytes.end());

    // Anexar D_FILE: 1 byte 0x76, seguido de 24 linhas de (32 espacos 0x00 + 1 byte 0x76)
    p_file.push_back(0x76);
    for (int r = 0; r < 24; ++r) {
        p_file.insert(p_file.end(), 32, 0x00);
        p_file.push_back(0x76);
    }

    // Anexar VARS: 2 bytes (0x80, 0x76)
    p_file.push_back(0x80);
    p_file.push_back(0x76);

    result.p_data = std::move(p_file);
    result.total_p_bytes = result.p_data.size();
    result.success = true;

    return result;
}

CompilerResult ZXText2P::compile_file(const std::string& input_path, const std::string& output_p_path, const CompilerOptions& options) {
    std::ifstream in(input_path, std::ios::in | std::ios::binary);
    if (!in) {
        CompilerResult res;
        res.success = false;
        res.error_message = "Nao foi possivel abrir o arquivo de entrada: " + input_path;
        res.error_line = 0;
        return res;
    }

    std::ostringstream ss;
    ss << in.rdbuf();
    std::string source = ss.str();
    in.close();

    CompilerResult res = compile_source(source, options);
    if (res.success && !output_p_path.empty()) {
        if (!save_p_file(output_p_path, res.p_data)) {
            res.success = false;
            res.error_message = "Falha ao gravar arquivo binario de saida: " + output_p_path;
        }
    }

    return res;
}

bool ZXText2P::save_p_file(const std::string& output_path, const std::vector<uint8_t>& p_data) {
    std::ofstream out(output_path, std::ios::out | std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(p_data.data()), p_data.size());
    out.close();
    return true;
}

} // namespace compiler
} // namespace zx81ide
