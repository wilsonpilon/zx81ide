#ifndef ZX81IDE_COMPILER_ZXTEXT2P_H
#define ZX81IDE_COMPILER_ZXTEXT2P_H

#include <string>
#include <vector>
#include <cstdint>

namespace zx81ide {
namespace compiler {

// Estrutura de opcoes de compilacao compativeis com ZXText2P
struct CompilerOptions {
    bool use_labels = false;      // Modo labels ao inves de numeros de linha (-l)
    int start_line = 10;          // Linha inicial no modo labels (-s)
    int line_increment = 2;       // Incremento de linha no modo labels (-i)
    std::string output_file = ""; // Arquivo de saida (.p) opcional
};

// Estrutura de resultado da compilacao
struct CompilerResult {
    bool success = false;
    std::string error_message;
    int error_line = 0;              // Linha original do arquivo onde ocorreu o erro
    std::vector<uint8_t> p_data;     // Dados binarios completos do arquivo .P
    int total_lines = 0;             // Total de linhas BASIC compiladas
    size_t program_bytes = 0;        // Tamanho da area de codigo BASIC
    size_t total_p_bytes = 0;        // Tamanho total do arquivo .P
};

// Estrutura de definicao de caractere grafico do Sinclair ZX-81
struct GraphicCharDef {
    int zx_code;                     // Codigo interno ZX-81 (0..138)
    std::string escape_seq;          // Sequencia escape ZXText2P (ex: "\\:'", "\\::")
    std::string glyph_utf8;          // Glifo visual correspondente em UTF-8
    std::string description;         // Descricao em portugues
};

class ZXText2P {
public:
    // Compila a partir de uma string de codigo fonte BASIC
    static CompilerResult compile_source(const std::string& source_code, const CompilerOptions& options = CompilerOptions());

    // Compila a partir de um arquivo em disco e opcionalmente grava o .P
    static CompilerResult compile_file(const std::string& input_path, const std::string& output_p_path, const CompilerOptions& options = CompilerOptions());

    // Salva os dados binarios compilados em um arquivo .P no disco
    static bool save_p_file(const std::string& output_path, const std::vector<uint8_t>& p_data);

    // Retorna a tabela completa de caracteres graficos suportados pelo ZXText2P
    static const std::vector<GraphicCharDef>& get_graphics_table();

    // Converte numero de ponto flutuante para o formato interno de 5 bytes do ZX-81
    static bool double_to_zx81_fp(double num, int& exp, uint32_t& mantissa);

    // Converte caractere ASCII para codigo ZX-81 (0..63)
    static uint8_t ascii_to_zx(char c);

    // Verifica se uma palavra e palavra reservada (token) do ZX-81
    static bool is_keyword(const std::string& word);

    // Retorna a lista completa de palavras reservadas / tokens do ZX-81
    static const std::vector<std::string>& get_all_keywords();
};

} // namespace compiler
} // namespace zx81ide

#endif // ZX81IDE_COMPILER_ZXTEXT2P_H
