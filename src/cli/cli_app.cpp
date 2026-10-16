#include "cli/cli_app.h"
#include "cli/cli_parser.h"
#include "ui_tui/tui_app.h"
#include "core/version.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

namespace zx81ide {
namespace cli {

namespace {

void print_cli_banner() {
    std::cout << "================================================================" << std::endl;
    std::cout << "  _____  _     ___  _   _     _ _       " << std::endl;
    std::cout << " |_  / \\/ /___( _ )/ | (_) __| (_) ___  " << std::endl;
    std::cout << "  / / \\  // _ \\/ _ \\| | | |/ _` | |/ _ \\ " << std::endl;
    std::cout << " /___//_/\\_\\___/\\___/|_| |_|\\__,_|_|\\___/ " << std::endl;
    std::cout << "  Integrated Development Environment for Sinclair ZX-81 & Clones" << std::endl;
    std::cout << "================================================================" << std::endl;
    std::cout << "Console CLI Interativo - zx81ide " << get_full_version_string() << std::endl;
    std::cout << "Digite 'help' para comandos ou 'tui' para o modo grafico retro." << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
}

void print_cli_commands() {
    std::cout << "\nComandos Disponiveis no Console CLI:\n";
    std::cout << "  help, ?           Exibe esta lista de comandos\n";
    std::cout << "  tui               Inicia a interface retro MS-DOS em modo texto (TUI)\n";
    std::cout << "  version, ver      Exibe os detalhes completos de versao e codenames\n";
    std::cout << "  clear, cls        Limpa a tela do console\n";
    std::cout << "  exit, quit, q     Encerra o zx81ide\n\n";
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

} // anonymous namespace

int run_interactive_cli() {
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

        std::string lower_cmd = cmd;
        std::transform(lower_cmd.begin(), lower_cmd.end(), lower_cmd.begin(), ::tolower);

        if (lower_cmd == "help" || lower_cmd == "?") {
            print_cli_commands();
        } else if (lower_cmd == "tui") {
            std::cout << "\nIniciando modo TUI retro... Pressione qualquer tecla para continuar se solicitado.\n";
            zx81ide::ui_tui::run_tui();
            // Ao retornar da TUI, restaura tela e exibe mensagem
            std::cout << "\n[Retornando ao console CLI zx81ide]\n\n";
        } else if (lower_cmd == "version" || lower_cmd == "ver") {
            print_version();
        } else if (lower_cmd == "clear" || lower_cmd == "cls") {
            std::cout << "\x1b[2J\x1b[H";
        } else if (lower_cmd == "exit" || lower_cmd == "quit" || lower_cmd == "q") {
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
