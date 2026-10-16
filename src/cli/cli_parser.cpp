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
        } else if (!arg.empty() && arg[0] == '-') {
            options.has_error = true;
            options.error_message = "Opcao desconhecida: " + arg + "\nUse 'zx81ide --help' para ver as opcoes validas.";
            return options;
        } else {
            options.source_file = arg;
        }
    }

    return options;
}

void print_help() {
    std::cout << "================================================================" << std::endl;
    std::cout << " zx81ide - IDE para Sinclair ZX-81 e Clones" << std::endl;
    std::cout << " Versao: " << get_version_string() << " [" << VERSION_GAME_CODENAME << "]" << std::endl;
    std::cout << "================================================================" << std::endl;
    std::cout << "Uso:\n";
    std::cout << "  zx81ide                       Inicia no modo interativo CLI (padrao)\n";
    std::cout << "  zx81ide [opcoes] [arquivo]    Executa opcao ou modo especifico\n\n";
    std::cout << "Opcoes de Linha de Comando:\n";
    std::cout << "  -h, --help                    Exibe esta mensagem de ajuda e encerra\n";
    std::cout << "  -v, --version                 Exibe informacoes de versao e encerra\n";
    std::cout << "  --cli                         Inicia no console interativo CLI (padrao)\n";
    std::cout << "  --tui                         Inicia no modo TUI (Text User Interface retro)\n";
    std::cout << "  --gui                         Inicia no modo GUI (interface desktop grafic)\n\n";
    std::cout << "No modo interativo CLI, digite 'help' para comandos internos (ex: 'tui', 'version', 'exit').\n";
    std::cout << "================================================================" << std::endl;
}

void print_version() {
    std::cout << get_full_version_string() << std::endl;
    std::cout << "Hardware Milestone (X): " << VERSION_MAJOR_NAME << std::endl;
    std::cout << "Game Codename      (Y): " << VERSION_GAME_CODENAME << std::endl;
    std::cout << "Build Subtitle     (Z): " << VERSION_BUILD_SUBTITLE << std::endl;
}

} // namespace cli
} // namespace zx81ide
