#include "cli/cli_parser.h"
#include "cli/cli_app.h"
#include "ui_tui/tui_app.h"
#include "editor/text_editor.h"
#include "core/config_manager.h"
#include "core/project.h"
#include <iostream>

int main(int argc, char* argv[]) {
    using namespace zx81ide::cli;

    CliOptions options = parse_arguments(argc, argv);

    if (options.has_error) {
        std::cerr << "Erro: " << options.error_message << std::endl;
        return 1;
    }

    // Inicializar ConfigManager SQLite
    auto& cfg = zx81ide::core::ConfigManager::instance();
    cfg.initialize(options.config_db_path);

    // Se houve --theme na linha de comando
    if (!options.theme_override.empty()) {
        if (cfg.set_theme(options.theme_override)) {
            std::cout << "[OK] Tema atualizado para '" << options.theme_override << "' no banco SQLite.\n";
        } else {
            std::cerr << "[ERRO] Tema '" << options.theme_override << "' invalido. Use: normal, invertido, claro, escuro.\n";
            return 1;
        }
        if (options.mode == AppMode::CLI_INTERACTIVE && argc <= 3) {
            // Se apenas definiu o tema pela CLI, finaliza
            return 0;
        }
    }

    switch (options.mode) {
        case AppMode::CLI_HELP:
            print_help();
            return 0;

        case AppMode::CLI_VERSION:
            print_version();
            return 0;

        case AppMode::PROJECT_INFO: {
            zx81ide::core::Project proj(options.project_file);
            if (!proj.is_open()) {
                std::cerr << "[ERRO] Nao foi possivel abrir projeto: " << proj.last_error() << std::endl;
                return 1;
            }
            auto summary = proj.get_summary();
            std::cout << "\n================================================================" << std::endl;
            std::cout << " Projeto: " << summary.meta.name << " (" << options.project_file << ")" << std::endl;
            std::cout << "================================================================" << std::endl;
            std::cout << "  Versao          : " << summary.meta.version << std::endl;
            std::cout << "  Autor           : " << summary.meta.author << std::endl;
            std::cout << "  Modelo Alvo     : " << summary.meta.target_model << std::endl;
            std::cout << "  Fonte Principal : " << summary.meta.default_source << std::endl;
            std::cout << "  Criado em       : " << summary.meta.created_at << std::endl;
            std::cout << "  Atualizado em   : " << summary.meta.updated_at << std::endl;
            std::cout << "  --------------------------------------------------------------" << std::endl;
            std::cout << "  Fontes (BASIC/ASM) : " << summary.source_count << std::endl;
            std::cout << "  Telas/Screens      : " << summary.screen_count << std::endl;
            std::cout << "  Binarios (.p/.bin) : " << summary.binary_count << std::endl;
            std::cout << "  Manuais/Documentos : " << summary.manual_count << std::endl;
            std::cout << "================================================================\n" << std::endl;
            return 0;
        }

        case AppMode::TUI:
            cfg.prompt_theme_selection_if_needed();
            return zx81ide::ui_tui::run_tui();

        case AppMode::GUI:
            std::cout << "Modo GUI desktop (Dear ImGui) esta em fase de planejamento/desenvolvimento.\n";
            std::cout << "Iniciando modo TUI retro alternativo...\n\n";
            cfg.prompt_theme_selection_if_needed();
            return zx81ide::ui_tui::run_tui();

        case AppMode::EDITOR:
            cfg.prompt_theme_selection_if_needed();
            return zx81ide::editor::run_text_editor(options.source_file.empty() ? "NONAME.BAS" : options.source_file);

        case AppMode::CLI_INTERACTIVE:
        default:
            return run_interactive_cli();
    }
}
