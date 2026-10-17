#ifndef ZX81IDE_CLI_PARSER_H
#define ZX81IDE_CLI_PARSER_H

#include <string>
#include <vector>

namespace zx81ide {
namespace cli {

enum class AppMode {
    CLI_INTERACTIVE,
    CLI_HELP,
    CLI_VERSION,
    TUI,
    GUI,
    PROJECT_INFO,
    EDITOR
};

struct CliOptions {
    AppMode mode = AppMode::CLI_INTERACTIVE;
    std::string source_file;
    std::string project_file;
    std::string theme_override;
    std::string config_db_path;
    bool has_error = false;
    std::string error_message;
};

CliOptions parse_arguments(int argc, char* argv[]);
void print_help();
void print_version();

} // namespace cli
} // namespace zx81ide

#endif // ZX81IDE_CLI_PARSER_H
