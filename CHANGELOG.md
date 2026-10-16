# Changelog

Todas as alterações notáveis neste projeto serão documentadas neste arquivo.

O formato é baseado em [Keep a Changelog](https://keepachangelog.com/pt-BR/1.0.0/), e este projeto adere ao esquema de versionamento personalizado:
* **`X` (Major):** Nome e modelo clássico de ZX-81 e clones (ex: *Sinclair ZX81*, *Timex Sinclair 1000*, *Microdigital TK85*).
* **`Y` (Minor):** Nome de jogo clássico de ZX-81 (ex: *3D Monster Maze*, *Mazogs*, *Flight Simulation*).
* **`Z` (Patch/Build):** Subtítulo curto indicando o que foi alterado.

---

## [0.1.1] - 2026-10-16
### Codename: "3D Monster Maze"
### Subtítulo: "CLI interativo e modo TUI retro estilo MS-DOS"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Adicionado (Added)
* Módulo de parser de linha de comando CLI (`include/cli/cli_parser.h`, `src/cli/cli_parser.cpp`) com suporte a:
  * `--help` / `-h`: Exibição detalhada de parâmetros e sintaxe.
  * `--version` / `-v`: Informações completas de versão semântica, marcos de hardware e codenames.
  * `--tui`: Inicialização direta no ambiente gráfico de terminal (TUI).
  * `--cli`: Entrada explícita no console interativo (comportamento padrão ao iniciar sem parâmetros).
* Console interativo CLI (`include/cli/cli_app.h`, `src/cli/cli_app.cpp`):
  * Prompt de comando dedicado `zx81ide>`.
  * Comandos internos integrados: `help` / `?`, `tui`, `version` / `ver`, `clear` / `cls`, `exit` / `quit` / `q`.
* Ambiente TUI (Text User Interface) retro estilo MS-DOS anos 1990 (`include/ui_tui/tui_app.h`, `src/ui_tui/tui_app.cpp`):
  * **Paleta estrita Sinclair ZX-81:** Respeito integral à estética monocromática/escala de cinza usando exclusivamente **Preto**, **Branco** e **Cinza**.
  * **Barra de Menus (Topo):** Fundo preto e letras em branco com suporte a dropdowns:
    * `Arquivo` -> `Sair` (retorna ao CLI / encerra).
    * `Help` -> `About` (abre janela modal com metadados).
  * **Área de Desktop:** Padrão quadriculado ASCII em cinza e branco (`░░▒▒`) com sombras retro nos elementos suspensos.
  * **Barra de Status (Rodapé):** Fundo cinza e letras em preto com teclas de atalho e tag de versão alinhada à direita (`| v0.1.1`).
  * **Janela Modal de Sobre (About):** Moldura dupla clássica (`╔═╗`, `║ ║`, `╚═╝`), sombra em relevo e botão `[ OK ]`.
  * **Navegação Completa por Teclado:** Suporte a setas direcionais, Enter, Esc, F10, F1, atalhos rápidos (`Alt+X`, `A`, `H`).

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
