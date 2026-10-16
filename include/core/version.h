#ifndef ZX81IDE_VERSION_H
#define ZX81IDE_VERSION_H

#include <string>

namespace zx81ide {

constexpr int VERSION_MAJOR = 0;
constexpr int VERSION_MINOR = 1;
constexpr int VERSION_PATCH = 3;

// X: Nomes e modelos importantes de ZX-81 e clones
inline const std::string VERSION_MAJOR_NAME = "Sinclair ZX81 (Foundation)";

// Y: Nome de um jogo de ZX-81 clássico
inline const std::string VERSION_GAME_CODENAME = "3D Monster Maze";

// Z: Subtítulo curto indicando o que foi alterado
inline const std::string VERSION_BUILD_SUBTITLE = "Correcao no parse UTF-8 das bordas no TUI";

inline std::string get_version_string() {
    return std::to_string(VERSION_MAJOR) + "." +
           std::to_string(VERSION_MINOR) + "." +
           std::to_string(VERSION_PATCH);
}

inline std::string get_full_version_string() {
    return "zx81ide v" + get_version_string() + " [" + VERSION_GAME_CODENAME +
           "] - " + VERSION_BUILD_SUBTITLE;
}

} // namespace zx81ide

#endif // ZX81IDE_VERSION_H
