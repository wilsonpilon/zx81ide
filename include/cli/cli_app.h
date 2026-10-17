#ifndef ZX81IDE_CLI_APP_H
#define ZX81IDE_CLI_APP_H

#include <string>
#include <vector>

namespace zx81ide {
namespace cli {

int run_interactive_cli();
void handle_compile_command(const std::vector<std::string>& tokens);
void handle_compile_chars_command();

} // namespace cli
} // namespace zx81ide

#endif // ZX81IDE_CLI_APP_H
