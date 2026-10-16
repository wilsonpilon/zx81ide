#include "cli/cli_parser.h"
#include "cli/cli_app.h"
#include "ui_tui/tui_app.h"
#include <iostream>

int main(int argc, char* argv[]) {
    using namespace zx81ide::cli;

    CliOptions options = parse_arguments(argc, argv);

    if (options.has_error) {
        std::cerr << "Erro: " << options.error_message << std::endl;
        return 1;
    }

    switch (options.mode) {
        case AppMode::CLI_HELP:
            print_help();
            return 0;

        case AppMode::CLI_VERSION:
            print_version();
            return 0;

        case AppMode::TUI:
            return zx81ide::ui_tui::run_tui();

        case AppMode::GUI:
            std::cout << "Modo GUI desktop (Dear ImGui) esta em fase de planejamento/desenvolvimento.\n";
            std::cout << "Iniciando modo TUI retro alternativo...\n\n";
            return zx81ide::ui_tui::run_tui();

        case AppMode::CLI_INTERACTIVE:
        default:
            return run_interactive_cli();
    }
}
