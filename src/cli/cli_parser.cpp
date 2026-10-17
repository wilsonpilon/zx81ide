#include "cli/cli_parser.h"
#include "core/version.h"
#include <iostream>
#include <iomanip>

namespace zx81ide {
namespace cli {

CliOptions parse_arguments(int argc, char* argv[]) {
    CliOptions options;

    if (argc <= 1) {
        options.mode = AppMode::CLI_INTERACTIVE;
        return options;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            options.mode = AppMode::CLI_HELP;
            return options;
        } else if (arg == "-v" || arg == "--version") {
            options.mode = AppMode::CLI_VERSION;
            return options;
        } else if (arg == "--tui") {
            options.mode = AppMode::TUI;
        } else if (arg == "--gui") {
            options.mode = AppMode::GUI;
        } else if (arg == "--cli") {
            options.mode = AppMode::CLI_INTERACTIVE;
        } else if (arg == "--theme") {
            if (i + 1 < argc) {
                options.theme_override = argv[++i];
            } else {
                options.has_error = true;
                options.error_message = "Opcao '--theme' requer o nome do tema (normal, invertido, claro, escuro).";
                return options;
            }
        } else if (arg == "--config-db") {
            if (i + 1 < argc) {
                options.config_db_path = argv[++i];
            } else {
                options.has_error = true;
                options.error_message = "Opcao '--config-db' requer o caminho do arquivo SQLite.";
                return options;
            }
        } else if (arg == "--project") {
            if (i + 1 < argc) {
                options.project_file = argv[++i];
                options.mode = AppMode::PROJECT_INFO;
            } else {
                options.has_error = true;
                options.error_message = "Opcao '--project' requer o caminho do arquivo .zx81project.";
                return options;
            }
        } else if (!arg.empty() && arg[0] == '-') {
            options.has_error = true;
            options.error_message = "Opcao desconhecida: " + arg + "\nUse 'zx81ide --help' para ver as opcoes validas.";
            return options;
        } else {
            // Se terminar com .zx81project, tratar como projeto
            if (arg.size() > 12 && arg.substr(arg.size() - 12) == ".zx81project") {
                options.project_file = arg;
                options.mode = AppMode::PROJECT_INFO;
            } else {
                options.source_file = arg;
            }
        }
    }

    return options;
}

void print_help() {
    std::cout << "================================================================" << std::endl;
    std::cout << " zx81ide - IDE para Sinclair ZX-81 e Clones" << std::endl;
    std::cout << " Versao: " << get_version_string() << " [" << VERSION_GAME_CODENAME << "]" << std::endl;
    std::cout << " Subtitulo: " << VERSION_BUILD_SUBTITLE << std::endl;
    std::cout << "================================================================" << std::endl;
    std::cout << "Uso:\n";
    std::cout << "  zx81ide                             Inicia no modo interativo CLI (padrao)\n";
    std::cout << "  zx81ide [opcoes]                    Executa opcao ou modo especifico\n";
    std::cout << "  zx81ide <projeto.zx81project>       Inspeciona resumo de um projeto\n\n";
    std::cout << "Opcoes de Linha de Comando:\n";
    std::cout << "  -h, --help                          Exibe esta mensagem de ajuda e encerra\n";
    std::cout << "  -v, --version                       Exibe informacoes de versao e encerra\n";
    std::cout << "  --cli                               Inicia no console interativo CLI (padrao)\n";
    std::cout << "  --tui                               Inicia no modo TUI (Text User Interface retro)\n";
    std::cout << "  --gui                               Inicia no modo GUI (interface desktop grafica)\n";
    std::cout << "  --theme <nome>                      Define ou altera o tema no banco (normal, invertido, claro, escuro)\n";
    std::cout << "  --config-db <caminho.db>            Especifica arquivo customizado de banco de configuracoes SQLite\n";
    std::cout << "  --project <arquivo.zx81project>     Exibe o conteudo e estatisticas de um arquivo de projeto SQLite\n\n";
    std::cout << "Recursos Integrados de Banco de Dados SQLite:\n";
    std::cout << "  * Configuracoes: Armazenadas em SQLite (zx81ide.db) incluindo gestao de temas.\n";
    std::cout << "  * Projetos: Arquivos '.zx81project' sao bancos SQLite portateis contendo fontes,\n";
    std::cout << "    telas (DFILE/ASCII), binarios compilados (.p), manuais e configuracoes.\n\n";
    std::cout << "No modo interativo CLI, digite 'help' para comandos internos (ex: 'theme', 'config', 'project', 'tui', 'exit').\n";
    std::cout << "================================================================" << std::endl;
}

void print_version() {
    std::cout << get_full_version_string() << std::endl;
    std::cout << "Hardware Milestone (X): " << VERSION_MAJOR_NAME << std::endl;
    std::cout << "Game Codename      (Y): " << VERSION_GAME_CODENAME << std::endl;
    std::cout << "Build Subtitle     (Z): " << VERSION_BUILD_SUBTITLE << std::endl;
    std::cout << "Storage Engine        : SQLite 3 (Amalgamation) para Configs e .zx81project" << std::endl;
}

} // namespace cli
} // namespace zx81ide
