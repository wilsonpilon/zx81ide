#include <iostream>
#include "core/version.h"

void print_banner() {
    std::cout << "================================================================" << std::endl;
    std::cout << "  _____  _     ___  _   _     _ _       " << std::endl;
    std::cout << " |_  / \\/ /___( _ )/ | (_) __| (_) ___  " << std::endl;
    std::cout << "  / / \\  // _ \\/ _ \\| | | |/ _` | |/ _ \\ " << std::endl;
    std::cout << " /___//_/\\_\\___/\\___/|_| |_|\\__,_|_|\\___/ " << std::endl;
    std::cout << "  Integrated Development Environment for Sinclair ZX-81 & Clones" << std::endl;
    std::cout << "================================================================" << std::endl;
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    print_banner();

    std::cout << "Program Name   : zx81ide" << std::endl;
    std::cout << "Version        : " << zx81ide::get_version_string() << std::endl;
    std::cout << "Major Model (X): " << zx81ide::VERSION_MAJOR_NAME << std::endl;
    std::cout << "Game Codename(Y): " << zx81ide::VERSION_GAME_CODENAME << std::endl;
    std::cout << "Build Subtitle(Z): " << zx81ide::VERSION_BUILD_SUBTITLE << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "Hello, Sinclair ZX-81 World! [READY]" << std::endl;
    std::cout << "================================================================" << std::endl;

    return 0;
}
