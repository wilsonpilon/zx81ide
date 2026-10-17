# Changelog

Todas as alterações notáveis neste projeto serão documentadas neste arquivo.

O formato é baseado em [Keep a Changelog](https://keepachangelog.com/pt-BR/1.0.0/), e este projeto adere ao esquema de versionamento personalizado:
* **`X` (Major):** Nome e modelo clássico de ZX-81 e clones (ex: *Sinclair ZX81*, *Timex Sinclair 1000*, *Microdigital TK85*).
* **`Y` (Minor):** Nome de jogo clássico de ZX-81 (ex: *3D Monster Maze*, *Mazogs*, *Flight Simulation*).
* **`Z` (Patch/Build):** Subtítulo curto indicando o que foi alterado.

---

## [0.1.5] - 2026-10-16
### Codename: "3D Monster Maze"
### Subtítulo: "Integracao SQLite, Banco de Configuracoes, Temas e Sistema de Projetos .zx81project"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Adicionado (Added)
* **Integração Nativa do SQLite 3:** Incorporada a biblioteca oficial SQLite 3 Amalgamation em `third_party/sqlite3/`, compilada estaticamente com o projeto sem dependências externas de DLLs.
* **Camada RAII C++ para SQLite (`core/db.h`, `core/db.cpp`):** Classes `Database` e `Statement` fornecendo gerenciamento automático de memória, suporte a transações ACID, execução de queries escalares, binds seguros (texto, inteiros, blobs) e tratamento robusto de erros.
* **Gerenciador de Configurações SQLite (`core/config_manager.h`, `core/config_manager.cpp`):** Armazenamento de parâmetros globais do ambiente em banco SQLite (`zx81ide.db`), com tabela `settings` e versionamento temporal.
* **Seleção e Gestão de Temas:**
  * Diálogo interativo na inicialização caso nenhum tema esteja gravado no banco de configurações.
  * Suporte aos 4 temas solicitados: `normal` (Sinclair padrão), `invertido` (texto preto em fundo branco), `claro` e `escuro`.
  * Novos comandos de linha de comando: `theme` (inspeciona) e `theme <nome>` (altera), além de parâmetro `--theme <nome>` na inicialização.
* **Sistema de Projetos Portáteis `.zx81project` (`core/project.h`, `core/project.cpp`):**
  * Cada arquivo de projeto `.zx81project` é um banco SQLite estruturado contendo metadados (`project_meta`), configurações (`project_settings`), fontes de código BASIC e ASM (`sources`), telas DFILE/ASCII (`screens`), binários compilados (`binaries`), manuais e documentações em Markdown (`manuals`) e recursos gráficos (`assets`).
  * Comandos CLI de manipulação de projetos: `project new`, `project info`, `project list`, `project add-source`, `project view-source`, `project export-source`, `project add-doc`, `project view-doc`.
* **Integração Visual na TUI:** Indicador dinâmico do tema ativo na barra de status inferior (`| Tema: <nome> | v0.1.5`) e identificação no diálogo About.

---

## [0.1.4] - 2026-10-16
### Codename: "3D Monster Maze"
### Subtítulo: "Alinhamento e ampliacao da borda dos menus TUI"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Corrigido (Fixed)
* Ampliada a largura da caixa dos menus suspensos (dropdown) de 20 para 22 colunas e implementado o formatador `format_menu_item`, garantindo que os textos e atalhos fiquem estritamente contidos na área interna (`inner_w = 20`) sem sobrepor ou cortar a borda lateral direita (`│`).
* Incluído respiro simétrico nas extremidades de cada item de menu, oferecendo acabamento visual limpo e legível.

---

## [0.1.3] - 2026-10-16
### Codename: "3D Monster Maze"
### Subtítulo: "Correcao no parse UTF-8 das bordas no TUI"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Corrigido (Fixed)
* Corrigido o fatiamento indevido de bytes em caracteres multi-byte UTF-8 na função `put_string`, que dividia símbolos gráficos de moldura (`┌`, `─`, `┐`, `│`, `└`, `┘`, `╔`, `═`, `╗`, `╚`, `╝`, `║`) gerando caracteres inválidos de substituição (losango com ponto de interrogação) em fontes Nerd Fonts (JetBrains Mono) e terminais modernos.
* Introduzida a rotina `repeat_utf8` para garantir que o preenchimento de bordas mantenha contagem estrita de colunas visuais no console.
* Adicionado `SetConsoleCP(CP_UTF8)` no bootstrap do console Windows para sincronização plena de codificação de entrada e saída.

---

## [0.1.2] - 2026-10-16
### Codename: "3D Monster Maze"
### Subtítulo: "Fundo uniforme com quadriculado no desktop TUI"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Alterado (Changed)
* Ajustada a renderização da área de desktop no modo TUI para utilizar uma cor de fundo estritamente uniforme e contínua, eliminando a alternância xadrez claro/escuro e mantendo o caractere ASCII quadriculado (`░`) texturizado em toda a tela.

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
