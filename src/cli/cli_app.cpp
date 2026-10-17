#include "cli/cli_app.h"
#include "cli/cli_parser.h"
#include "ui_tui/tui_app.h"
#include "editor/text_editor.h"
#include "core/version.h"
#include "core/config_manager.h"
#include "core/project.h"
#include "compiler/zxtext2p.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

namespace zx81ide {
namespace cli {

namespace {

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> split_words(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string read_file_to_string(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::in | std::ios::binary);
    if (!file) return "";
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

bool write_string_to_file(const std::string& file_path, const std::string& content) {
    std::ofstream file(file_path, std::ios::out | std::ios::binary);
    if (!file) return false;
    file << content;
    return true;
}

void print_cli_banner() {
    auto& cfg = core::ConfigManager::instance();
    std::string current_theme = cfg.get_theme();

    std::cout << "================================================================" << std::endl;
    std::cout << "  _____  _     ___  _   _     _ _       " << std::endl;
    std::cout << " |_  / \\/ /___( _ )/ | (_) __| (_) ___  " << std::endl;
    std::cout << "  / / \\  // _ \\/ _ \\| | | |/ _` | |/ _ \\ " << std::endl;
    std::cout << " /___//_/\\_\\___/\\___/|_| |_|\\__,_|_|\\___/ " << std::endl;
    std::cout << "  Integrated Development Environment for Sinclair ZX-81 & Clones" << std::endl;
    std::cout << "================================================================" << std::endl;
    std::cout << "Console CLI Interativo - zx81ide " << get_full_version_string() << std::endl;
    std::cout << "Banco de Configs : " << cfg.get_database_path() << " (SQLite 3)" << std::endl;
    std::cout << "Tema Ativo       : " << current_theme << std::endl;
    std::cout << "Digite 'help' para comandos ou 'tui' para o modo grafico retro." << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
}

void print_cli_commands() {
    std::cout << "\nComandos Disponiveis no Console CLI:\n";
    std::cout << "  Ambiente e Modos:\n";
    std::cout << "    editor <arquivo.bas>              Abre o editor de texto livre Turbo C++ no console\n";
    std::cout << "    tui                               Inicia o modo TUI retro MS-DOS em tela cheia\n";
    std::cout << "    help, ?                           Exibe esta lista de comandos\n";
    std::cout << "    version, ver                      Exibe os detalhes de versao e codenames\n";
    std::cout << "    clear, cls                        Limpa a tela do console\n";
    std::cout << "    exit, quit, q                     Encerra o zx81ide\n\n";

    std::cout << "  Compilador ZXText2P C++ Nativo (BASIC -> .P):\n";
    std::cout << "    compile <arquivo.bas> [-o saida.p] [-l] [-s inicio] [-i incr]\n";
    std::cout << "                                      Compila codigo BASIC/labels diretamente para .P\n";
    std::cout << "    compile-chars                     Lista caracteres graficos ZX-81 e codigos de escape\n\n";

    std::cout << "  Gestao de Temas e Configuracoes SQLite:\n";
    std::cout << "    theme                             Exibe o tema atual e temas disponiveis\n";
    std::cout << "    theme <normal|invertido|claro|escuro>  Altera o tema ativo no banco SQLite\n";
    std::cout << "    config                            Lista todas as configuracoes salvas\n";
    std::cout << "    config get <chave>                Obtem o valor de uma configuracao\n";
    std::cout << "    config set <chave> <valor>        Salva/atualiza uma configuracao no banco\n\n";

    std::cout << "  Sistema de Projetos SQLite (.zx81project):\n";
    std::cout << "    project new <arquivo.zx81project> [nome] [autor]\n";
    std::cout << "                                      Cria novo projeto SQLite com estrutura inicial\n";
    std::cout << "    project info <arquivo.zx81project>  Exibe metadados e estatisticas do projeto\n";
    std::cout << "    project list <arquivo.zx81project>  Lista arquivos de codigo, telas e manuais\n";
    std::cout << "    project add-source <proj.zx81project> <arquivo_disco> [BASIC|ASM]\n";
    std::cout << "                                      Importa arquivo de codigo para o projeto\n";
    std::cout << "    project view-source <proj.zx81project> <nome_fonte>\n";
    std::cout << "                                      Exibe o codigo fonte gravado no projeto\n";
    std::cout << "    project export-source <proj.zx81project> <nome_fonte> <arquivo_saida>\n";
    std::cout << "                                      Exporta o fonte do projeto para o disco\n";
    std::cout << "    project add-doc <proj.zx81project> <arquivo_disco>\n";
    std::cout << "                                      Importa documentacao/manual para o projeto\n";
    std::cout << "    project view-doc <proj.zx81project> <titulo>\n";
    std::cout << "                                      Exibe manual/documentacao do projeto\n\n";
}

void handle_theme_command(const std::vector<std::string>& tokens) {
    auto& cfg = core::ConfigManager::instance();
    if (tokens.size() == 1) {
        std::cout << "\n[Tema do zx81ide]\n";
        std::cout << "Tema atualmente configurado: " << cfg.get_theme() << "\n";
        std::cout << "Temas disponiveis:\n";
        for (const auto& t : core::ConfigManager::get_available_themes()) {
            std::cout << "  - " << t << (t == cfg.get_theme() ? " (ativo)" : "") << "\n";
        }
        std::cout << "Para alterar, use: theme <nome_do_tema>\n\n";
        return;
    }

    std::string new_theme = tokens[1];
    std::transform(new_theme.begin(), new_theme.end(), new_theme.begin(), ::tolower);

    if (!core::ConfigManager::is_valid_theme(new_theme)) {
        std::cout << "[ERRO] Tema '" << new_theme << "' invalido. Escolha entre: normal, invertido, claro, escuro.\n";
        return;
    }

    if (cfg.set_theme(new_theme)) {
        std::cout << "[OK] Tema atualizado com sucesso para '" << new_theme << "' no banco SQLite (" << cfg.get_database_path() << ")!\n";
    } else {
        std::cout << "[ERRO] Falha ao salvar tema no banco de dados.\n";
    }
}

void handle_config_command(const std::vector<std::string>& tokens) {
    auto& cfg = core::ConfigManager::instance();

    if (tokens.size() == 1 || (tokens.size() == 2 && (tokens[1] == "list" || tokens[1] == "ls"))) {
        auto all = cfg.get_all();
        std::cout << "\n[Configuracoes zx81ide - " << cfg.get_database_path() << "]\n";
        if (all.empty()) {
            std::cout << "  (Nenhuma configuracao gravada)\n";
        } else {
            for (const auto& pair : all) {
                std::cout << "  " << pair.first << " = " << pair.second << "\n";
            }
        }
        std::cout << "\n";
        return;
    }

    std::string sub = tokens[1];
    std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);

    if (sub == "get") {
        if (tokens.size() < 3) {
            std::cout << "Uso: config get <chave>\n";
            return;
        }
        std::string key = tokens[2];
        if (cfg.has(key)) {
            std::cout << key << " = " << cfg.get(key) << "\n";
        } else {
            std::cout << "Chave '" << key << "' nao encontrada no banco de configuracoes.\n";
        }
    } else if (sub == "set") {
        if (tokens.size() < 4) {
            std::cout << "Uso: config set <chave> <valor>\n";
            return;
        }
        std::string key = tokens[2];
        std::string val = tokens[3];
        if (cfg.set(key, val)) {
            std::cout << "[OK] Configuracao '" << key << "' salva como '" << val << "'.\n";
        } else {
            std::cout << "[ERRO] Falha ao gravar configuracao no banco.\n";
        }
    } else {
        std::cout << "Subcomando desconhecido: " << sub << ". Use: config [list | get <chave> | set <chave> <valor>]\n";
    }
}

void handle_project_command(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        std::cout << "Uso do comando project:\n";
        std::cout << "  project new <proj.zx81project> [nome] [autor]\n";
        std::cout << "  project info <proj.zx81project>\n";
        std::cout << "  project list <proj.zx81project>\n";
        std::cout << "  project add-source <proj.zx81project> <arquivo_disco> [BASIC|ASM]\n";
        std::cout << "  project view-source <proj.zx81project> <nome_fonte>\n";
        std::cout << "  project export-source <proj.zx81project> <nome_fonte> <arquivo_saida>\n";
        std::cout << "  project add-doc <proj.zx81project> <arquivo_disco>\n";
        std::cout << "  project view-doc <proj.zx81project> <titulo>\n";
        return;
    }

    std::string sub = tokens[1];
    std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);

    if (sub == "new") {
        if (tokens.size() < 3) {
            std::cout << "Uso: project new <caminho.zx81project> [nome] [autor]\n";
            return;
        }
        std::string path = tokens[2];
        std::string name = (tokens.size() >= 4) ? tokens[3] : "NovoProjetoZX81";
        std::string author = (tokens.size() >= 5) ? tokens[4] : "";

        core::Project proj;
        if (proj.create_new(path, name, author)) {
            std::cout << "\n[OK] Projeto criado com sucesso em: " << path << "\n";
            std::cout << "  Nome       : " << name << "\n";
            std::cout << "  Autor      : " << (author.empty() ? "(padrao)" : author) << "\n";
            std::cout << "  Formato    : Banco SQLite 3 (.zx81project)\n";
            std::cout << "  Fonte inicial gerado: 'main.bas' (ZX-81 BASIC)\n";
            std::cout << "  Manual inicial gerado: 'MANUAL.md'\n\n";
        } else {
            std::cout << "[ERRO] Falha ao criar projeto: " << proj.last_error() << "\n";
        }
    } else if (sub == "info") {
        if (tokens.size() < 3) {
            std::cout << "Uso: project info <caminho.zx81project>\n";
            return;
        }
        std::string path = tokens[2];
        core::Project proj(path);
        if (!proj.is_open()) {
            std::cout << "[ERRO] Nao foi possivel abrir o projeto: " << proj.last_error() << "\n";
            return;
        }
        auto summary = proj.get_summary();
        std::cout << "\n================================================================" << std::endl;
        std::cout << " Informacoes do Projeto: " << summary.meta.name << std::endl;
        std::cout << "================================================================" << std::endl;
        std::cout << "  Arquivo         : " << path << " (SQLite 3)\n";
        std::cout << "  Versao          : " << summary.meta.version << "\n";
        std::cout << "  Autor           : " << summary.meta.author << "\n";
        std::cout << "  Modelo Alvo     : " << summary.meta.target_model << "\n";
        std::cout << "  Fonte Principal : " << summary.meta.default_source << "\n";
        std::cout << "  Criado em       : " << summary.meta.created_at << "\n";
        std::cout << "  Atualizado em   : " << summary.meta.updated_at << "\n";
        std::cout << "  --------------------------------------------------------------\n";
        std::cout << "  Fontes (BASIC/ASM) : " << summary.source_count << "\n";
        std::cout << "  Telas/Screens      : " << summary.screen_count << "\n";
        std::cout << "  Binarios (.p/.bin) : " << summary.binary_count << "\n";
        std::cout << "  Manuais/Documentos : " << summary.manual_count << "\n";
        std::cout << "================================================================\n\n";
    } else if (sub == "list" || sub == "ls") {
        if (tokens.size() < 3) {
            std::cout << "Uso: project list <caminho.zx81project>\n";
            return;
        }
        std::string path = tokens[2];
        core::Project proj(path);
        if (!proj.is_open()) {
            std::cout << "[ERRO] Nao foi possivel abrir o projeto: " << proj.last_error() << "\n";
            return;
        }
        std::cout << "\n[Conteudo do Projeto: " << path << "]\n";
        std::cout << "--- Fontes de Codigo ---\n";
        auto sources = proj.list_sources();
        for (const auto& s : sources) {
            std::cout << "  [" << s.type << "] " << s.name << " (" << s.content.size() << " bytes)\n";
        }
        std::cout << "--- Telas/Screens ---\n";
        auto screens = proj.list_screens();
        for (const auto& sc : screens) {
            std::cout << "  [" << sc.format << "] " << sc.name << "\n";
        }
        std::cout << "--- Binarios Compilados ---\n";
        auto bins = proj.list_binaries();
        for (const auto& b : bins) {
            std::cout << "  [" << b.target_type << "] " << b.name << " (" << b.size_bytes << " bytes)\n";
        }
        std::cout << "--- Manuais e Documentos ---\n";
        auto docs = proj.list_manuals();
        for (const auto& d : docs) {
            std::cout << "  [" << d.format << "] " << d.title << " (" << d.content.size() << " bytes)\n";
        }
        std::cout << "\n";
    } else if (sub == "add-source") {
        if (tokens.size() < 4) {
            std::cout << "Uso: project add-source <proj.zx81project> <arquivo_externo> [BASIC|ASM]\n";
            return;
        }
        std::string proj_path = tokens[2];
        std::string ext_file = tokens[3];
        std::string content = read_file_to_string(ext_file);
        if (content.empty()) {
            std::cout << "[ERRO] Nao foi possivel ler o arquivo: " << ext_file << "\n";
            return;
        }

        std::string file_name = ext_file;
        size_t slash = file_name.find_last_of("/\\");
        if (slash != std::string::npos) file_name = file_name.substr(slash + 1);

        std::string type = (tokens.size() >= 5) ? tokens[4] : "BASIC";
        if (file_name.size() > 4 && file_name.substr(file_name.size() - 4) == ".asm") type = "ASM";

        core::Project proj(proj_path);
        if (proj.add_source(file_name, type, content)) {
            std::cout << "[OK] Arquivo fonte '" << file_name << "' (" << type << ") importado com sucesso para o projeto.\n";
        } else {
            std::cout << "[ERRO] Falha ao importar fonte: " << proj.last_error() << "\n";
        }
    } else if (sub == "view-source") {
        if (tokens.size() < 4) {
            std::cout << "Uso: project view-source <proj.zx81project> <nome_fonte>\n";
            return;
        }
        std::string proj_path = tokens[2];
        std::string source_name = tokens[3];

        core::Project proj(proj_path);
        core::SourceFile sf;
        if (proj.get_source(source_name, sf)) {
            std::cout << "\n--- Codigo Fonte: " << sf.name << " (" << sf.type << ") ---\n";
            std::cout << sf.content << "\n----------------------------------------\n";
        } else {
            std::cout << "[ERRO] Arquivo fonte '" << source_name << "' nao encontrado no projeto.\n";
        }
    } else if (sub == "export-source") {
        if (tokens.size() < 5) {
            std::cout << "Uso: project export-source <proj.zx81project> <nome_fonte> <arquivo_destino>\n";
            return;
        }
        std::string proj_path = tokens[2];
        std::string source_name = tokens[3];
        std::string dest_path = tokens[4];

        core::Project proj(proj_path);
        core::SourceFile sf;
        if (proj.get_source(source_name, sf)) {
            if (write_string_to_file(dest_path, sf.content)) {
                std::cout << "[OK] Fonte '" << source_name << "' exportado para '" << dest_path << "' com sucesso.\n";
            } else {
                std::cout << "[ERRO] Falha ao gravar arquivo de saida: " << dest_path << "\n";
            }
        } else {
            std::cout << "[ERRO] Arquivo fonte '" << source_name << "' nao encontrado no projeto.\n";
        }
    } else if (sub == "add-doc") {
        if (tokens.size() < 4) {
            std::cout << "Uso: project add-doc <proj.zx81project> <arquivo_externo>\n";
            return;
        }
        std::string proj_path = tokens[2];
        std::string ext_file = tokens[3];
        std::string content = read_file_to_string(ext_file);
        if (content.empty()) {
            std::cout << "[ERRO] Nao foi possivel ler o arquivo de documentacao: " << ext_file << "\n";
            return;
        }

        std::string doc_title = ext_file;
        size_t slash = doc_title.find_last_of("/\\");
        if (slash != std::string::npos) doc_title = doc_title.substr(slash + 1);

        core::Project proj(proj_path);
        if (proj.add_manual(doc_title, "MARKDOWN", content)) {
            std::cout << "[OK] Documentacao '" << doc_title << "' adicionada com sucesso ao projeto.\n";
        } else {
            std::cout << "[ERRO] Falha ao adicionar documento: " << proj.last_error() << "\n";
        }
    } else if (sub == "view-doc") {
        if (tokens.size() < 4) {
            std::cout << "Uso: project view-doc <proj.zx81project> <titulo>\n";
            return;
        }
        std::string proj_path = tokens[2];
        std::string doc_title = tokens[3];

        core::Project proj(proj_path);
        auto manuals = proj.list_manuals();
        bool found = false;
        for (const auto& m : manuals) {
            if (m.title == doc_title) {
                std::cout << "\n=== Documentacao: " << m.title << " ===\n";
                std::cout << m.content << "\n====================================\n";
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "[ERRO] Documento '" << doc_title << "' nao encontrado no projeto.\n";
        }
    } else {
        std::cout << "Subcomando de projeto desconhecido: " << sub << ". Digite 'help' para opcoes.\n";
    }
}

void handle_compile_chars_command() {
    const auto& table = compiler::ZXText2P::get_graphics_table();
    std::cout << "\n=== Caracteres Graficos e Escapes ZXText2P do Sinclair ZX-81 ===\n";
    std::cout << " Glifo | Escape | Codigo ZX | Descricao\n";
    std::cout << "-------+--------+-----------+-----------------------------------------\n";
    for (const auto& item : table) {
        std::cout << "   " << item.glyph_utf8 << "   |  " << item.escape_seq;
        if (item.escape_seq.size() < 4) std::cout << " ";
        std::cout << "  |   " << item.zx_code;
        if (item.zx_code < 10) std::cout << "       | ";
        else if (item.zx_code < 100) std::cout << "      | ";
        else std::cout << "     | ";
        std::cout << item.description << "\n";
    }
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "Dica: No editor de texto, pressione F4 ou Ctrl+G para abrir o seletor visual!\n\n";
}

void handle_compile_command(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        std::cout << "\nUso: compile <arquivo.bas> [-o <saida.p>] [-l] [-s <inicio>] [-i <incremento>]\n";
        std::cout << "Exemplos:\n";
        std::cout << "  compile jogo.bas\n";
        std::cout << "  compile jogo.bas -o jogo.p -l -s 10 -i 5\n";
        std::cout << "Opcoes:\n";
        std::cout << "  -o <arquivo.p>   Especifica o nome do arquivo .P de saida\n";
        std::cout << "  -l               Ativa o modo de labels (@loop: ... GOTO @loop)\n";
        std::cout << "  -s <linha>       Define a linha inicial no modo de labels (default: 10)\n";
        std::cout << "  -i <incremento>  Define o incremento entre linhas no modo de labels (default: 2)\n\n";
        return;
    }

    std::string in_file = tokens[1];
    std::string out_file = "";
    auto& cfg = core::ConfigManager::instance();
    compiler::CompilerOptions opts;
    opts.use_labels = cfg.get_compiler_labels_mode();
    opts.start_line = cfg.get_compiler_start_line();
    opts.line_increment = cfg.get_compiler_line_increment();

    for (size_t i = 2; i < tokens.size(); ++i) {
        if (tokens[i] == "-l") {
            opts.use_labels = true;
        } else if (tokens[i] == "-o" && i + 1 < tokens.size()) {
            out_file = tokens[++i];
        } else if (tokens[i] == "-s" && i + 1 < tokens.size()) {
            try { opts.start_line = std::stoi(tokens[++i]); } catch (...) {}
        } else if (tokens[i] == "-i" && i + 1 < tokens.size()) {
            try { opts.line_increment = std::stoi(tokens[++i]); } catch (...) {}
        }
    }

    if (out_file.empty()) {
        out_file = in_file;
        size_t dot = out_file.find_last_of('.');
        if (dot != std::string::npos) {
            out_file = out_file.substr(0, dot) + ".p";
        } else {
            out_file += ".p";
        }
    }

    std::cout << "[Compilador ZXText2P C++] Compilando '" << in_file << "' -> '" << out_file << "'...\n";
    if (opts.use_labels) {
        std::cout << "  Modo            : Labels ativado (inicio=" << opts.start_line 
                  << ", incremento=" << opts.line_increment << ")\n";
    } else {
        std::cout << "  Modo            : Numeracao explicita\n";
    }

    auto res = compiler::ZXText2P::compile_file(in_file, out_file, opts);
    if (res.success) {
        std::cout << "[OK] Compilacao concluida com sucesso!\n";
        std::cout << "  Arquivo de Saida: " << out_file << "\n";
        std::cout << "  Tamanho Total .P: " << res.total_p_bytes << " bytes\n";
        std::cout << "  Linhas BASIC    : " << res.total_lines << "\n";
        std::cout << "  Codigo BASIC    : " << res.program_bytes << " bytes\n";
        std::cout << "Arquivo .P pronto para uso em emuladores (EightyOne) ou gravacao em fita ZX-81!\n\n";
    } else {
        std::cout << "[ERRO DE COMPILACAO]\n";
        if (res.error_line > 0) {
            std::cout << "  Linha " << res.error_line << ": " << res.error_message << "\n\n";
        } else {
            std::cout << "  " << res.error_message << "\n\n";
        }
    }
}

} // anonymous namespace

int run_interactive_cli() {
    // 1. Inicializar banco de configuracoes SQLite
    auto& cfg = core::ConfigManager::instance();
    cfg.initialize();

    // 2. Solicitar tema inicial se nao configurado
    cfg.prompt_theme_selection_if_needed();

    // 3. Exibir banner com versao e tema
    print_cli_banner();

    std::string line;
    while (true) {
        std::cout << "zx81ide> ";
        if (!std::getline(std::cin, line)) {
            // EOF atingido
            std::cout << "\nEncerrando zx81ide.\n";
            break;
        }

        std::string cmd = trim(line);
        if (cmd.empty()) {
            continue;
        }

        auto tokens = split_words(cmd);
        if (tokens.empty()) continue;

        std::string primary_cmd = tokens[0];
        std::transform(primary_cmd.begin(), primary_cmd.end(), primary_cmd.begin(), ::tolower);

        if (primary_cmd == "help" || primary_cmd == "?") {
            print_cli_commands();
        } else if (primary_cmd == "theme" || primary_cmd == "tema") {
            handle_theme_command(tokens);
        } else if (primary_cmd == "config" || primary_cmd == "cfg") {
            handle_config_command(tokens);
        } else if (primary_cmd == "project" || primary_cmd == "proj") {
            handle_project_command(tokens);
        } else if (primary_cmd == "compile" || primary_cmd == "comp" || primary_cmd == "zxtext2p") {
            handle_compile_command(tokens);
        } else if (primary_cmd == "compile-chars" || primary_cmd == "chars") {
            handle_compile_chars_command();
        } else if (primary_cmd == "editor" || primary_cmd == "edit") {
            std::string file_to_edit = "NONAME.BAS";
            if (tokens.size() > 1) {
                file_to_edit = tokens[1];
            }
            zx81ide::editor::run_text_editor(file_to_edit);
            std::cout << "\n[Retornando ao console CLI zx81ide]\n\n";
        } else if (primary_cmd == "tui") {
            std::cout << "\nIniciando modo TUI retro... Pressione qualquer tecla para continuar se solicitado.\n";
            zx81ide::ui_tui::run_tui();
            std::cout << "\n[Retornando ao console CLI zx81ide]\n\n";
        } else if (primary_cmd == "version" || primary_cmd == "ver") {
            print_version();
        } else if (primary_cmd == "clear" || primary_cmd == "cls") {
            std::cout << "\x1b[2J\x1b[H";
        } else if (primary_cmd == "exit" || primary_cmd == "quit" || primary_cmd == "q") {
            std::cout << "Obrigado por usar o zx81ide. Ate logo!\n";
            break;
        } else {
            std::cout << "Comando nao reconhecido: '" << cmd << "'. Digite 'help' para ver os comandos validos.\n";
        }
    }

    return 0;
}

} // namespace cli
} // namespace zx81ide
