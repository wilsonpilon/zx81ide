# Changelog

Todas as alterações notáveis neste projeto serão documentadas neste arquivo.

O formato é baseado em [Keep a Changelog](https://keepachangelog.com/pt-BR/1.0.0/), e este projeto adere ao esquema de versionamento personalizado:
* **`X` (Major):** Nome e modelo clássico de ZX-81 e clones (ex: *Sinclair ZX81*, *Timex Sinclair 1000*, *Microdigital TK85*).
* **`Y` (Minor):** Nome de jogo clássico de ZX-81 (ex: *3D Monster Maze*, *Mazogs*, *Flight Simulation*).
* **`Z` (Patch/Build):** Subtítulo curto indicando o que foi alterado.

---

## [0.1.0] - 2026-10-16
### Codename: "3D Monster Maze"
### Subtítulo: "Estrutura inicial do projeto, documentação e CLI bootstrap"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Adicionado (Added)
* Estrutura de diretórios padrão C++17 (`include/`, `src/`, `third_party/`, `tests/`, `tools/`, `docs/`, `dist/`).
* Arquivo de cabeçalho `include/core/version.h` com definição semântica de versões (`X.Y.Z`), codenames de jogos e subtítulos.
* Ponto de entrada inicial em `src/main.cpp` com exibição de banner ASCII, metadados de versão e mensagem de boas-vindas.
* Arquivo mestre `CMakeLists.txt` configurado para C++17 com flags de advertência estritas e regras de instalação.
* Documentação viva completa:
  * `README.md`: Apresentação geral, stack tecnológica, créditos de terceiros e guia de compilação.
  * `SPEC.md`: Especificação técnica detalhada das ferramentas e arquitetura.
  * `MANUAL.md`: Manual inicial de usuário e desenvolvedor.
  * `OUTLINE.md`: Registro de memória e conversas com a IA para continuidade entre ambientes.
  * `CHANGELOG.md`: Resumo padronizado de mudanças por versão.
  * `RELEASE.md`: Detalhamento completo das notas de release.
* Script de automação em PowerShell `build.ps1` com:
  * Detecção automática de compiladores (CMake, GCC/g++, Clang).
  * Compilação e linking do executável `zx81ide.exe`.
  * Criação e povoamento automático do diretório `dist/`.
  * Geração do pacote compactado `zx81ide-v-0.1.0.zip`.
  * Suporte a parâmetros `-Clean` e `-Release`.
