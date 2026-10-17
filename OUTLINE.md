# OUTLINE.md - Registro de Interações e Memória do Projeto (AI Handoff)

> **Finalidade deste Documento:**
> Este arquivo registra as conversas, decisões técnicas, diretrizes acordadas e o estado evolutivo do projeto **zx81ide** em conjunto com a Inteligência Artificial (Google Gemini / Antigravity IDE). Ele serve como base de memória persistente para garantir a continuidade perfeita do projeto caso haja troca de computador, ambiente de desenvolvimento ou motor de IA.

---

## 1. Metadados do Projeto e Proprietário

* **Nome do Projeto:** `zx81ide`
* **Descrição:** Ambiente Integrado de Desenvolvimento (IDE) para Sinclair ZX-81 e Clones (TK82/83/85, CP200, TS1000).
* **Autor / Proprietário:** Wilson "Barney" Pilon (`wilsonpilon@outlook.com`)
* **Repositório GitHub:** `https://github.com/wilsonpilon/zx81ide`
* **Projeto Irmão / Inspiração:** [fwMSX](https://github.com/wilsonpilon/fwMSX)
* **Licença:** BSD 3-Clause

---

## 2. Decisões Arquiteturais Consolidadas

1. **Linguagem Base:** **C++17** (com interoperabilidade em C puro para núcleos de emulação e montadores legados).
2. **Modos de Operação Planejados:**
   * **CLI:** Linha de comando para automação, pipelines e compilação direta.
   * **TUI:** Terminal User Interface via **FTXUI**.
   * **GUI:** Graphical User Interface via **Dear ImGui**.
3. **Componentes Externos Selecionados:**
   * *Montador Z80:* **SJAsmPlus** (principal), **z80asm/z88dk** (alternativa).
   * *Emulador:* **EightyOne Core** (principal adaptado em C++), **ZEsarUX / Fuse Z80** (alternativa).
   * *Compilador BASIC:* **zmakebas** + pré-processador customizado de sintaxe estendida.
4. **Política de Versionamento Personalizada (`X.Y.Z`):**
   * **`X` (Major):** Grandes conjuntos de funcionalidades consolidadas.
     * *Codename:* Nomes e modelos importantes de Sinclair ZX-81 e clones (ex: `Sinclair ZX81`, `Timex Sinclair 1000`, `Microdigital TK82C`, `Microdigital TK83`, `Microdigital TK85`, `Prológica CP200`).
   * **`Y` (Minor):** Features novas iniciadas ou concluídas.
     * *Codename:* Nome de jogos clássicos de ZX-81 (ex: `3D Monster Maze`, `Mazogs`, `Flight Simulation`, `1K ZX Chess`, `Asteroids`).
   * **`Z` (Patch/Build):** Cada compilação ou iteração de código.
     * *Subtítulo:* Frase curta e clara indicando exatamente o que foi alterado.
5. **Automação de Build e Release:**
   * Script central: `build.ps1` (PowerShell).
   * Saída de distribuição em `dist/`.
   * Pacote de release compactado: `zx81ide-v-X.Y.Z.zip`.
   * Releases no GitHub com tags `vX.Y.Z` e notas sincronizadas com `RELEASE.md`.

---

## 3. Registro Cronológico de Sessões

### Sessão 01 - 16/10/2026
* **Ambiente:** Windows 11, Antigravity IDE (DeepMind), Modelo: Google Gemini.
* **Solicitação do Usuário:**
  * Leitura e análise do documento vivo `SPEC.md`.
  * Criação da árvore de diretórios C/C++ profissional.
  * Criação dos documentos vivos: `README.md`, `MANUAL.md`, `OUTLINE.md`, `CHANGELOG.md`, `RELEASE.md`.
  * Implementação da primeira versão do software: `src/main.cpp` com banner, identificador do programa, versão `0.1.0` e mensagem Hello.
  * Elaboração do script `build.ps1` com compilação, geração da pasta `dist/` e arquivo compactado `.zip` para release.
  * Inicialização do repositório Git, primeiro commit, push e criação de Release no GitHub.
* **Ações Executadas:**
  1. Análise detalhada do `SPEC.md` e do repositório remoto `wilsonpilon/zx81ide`.
  2. Criação dos diretórios modulares (`include/`, `src/`, `third_party/`, `tests/`, `tools/`, `docs/`, `dist/`).
  3. Criação de `include/core/version.h` com a lógica das versões `X.Y.Z`.
  4. Criação de `src/main.cpp` exibindo o banner retro e informações do sistema.
  5. Criação de `CMakeLists.txt` configurado para C++17 e alertas estritos.
  6. Redação completa dos arquivos `README.md`, `MANUAL.md`, `OUTLINE.md`, `CHANGELOG.md` e `RELEASE.md`.
  7. Criação do script de automação `build.ps1`.
  8. Execução do build local, teste da execução de `zx81ide.exe` e geração do pacote `zx81ide-v-0.1.0.zip`.
  9. Commit inicial, push para `origin main`, criação de tag `v0.1.0` e GitHub Release.

### Sessão 02 - 16/10/2026
* **Ambiente:** Windows 11, Antigravity IDE (DeepMind), Modelo: Google Gemini.
* **Solicitação do Usuário:**
  * O módulo principal deve ser a CLI: implementar passagem de argumentos (`--help`, `-h`, etc.).
  * Sem argumentos, entrar diretamente no modo interativo CLI.
  * O modo CLI possui o comando `help` e o comando `tui` para iniciar o modo TUI.
  * O modo TUI deve remeter à era MS-DOS anos 1990 (Turbo Vision / Borland C++), respeitando o ZX-81 usando **apenas Branco, Preto e Cinza**.
  * Barra de menu com fundo preto e texto branco: `Arquivo -> Sair` e `Help -> About`.
  * Área de desktop com quadriculados ASCII em cinza e branco (`░░▒▒`).
  * Linha de status no rodapé com fundo cinza e texto preto, terminando com a versão alinhada à direita (`| v0.1.1`).
* **Ações Executadas:**
  1. Criação de `include/cli/cli_parser.h` e `src/cli/cli_parser.cpp` com suporte a `--help`, `--version`, `--tui`, `--cli`.
  2. Criação de `include/cli/cli_app.h` e `src/cli/cli_app.cpp` com console interativo `zx81ide>` e comandos `help`, `tui`, `version`, `clear`, `exit`.
  3. Criação de `include/ui_tui/tui_app.h` e `src/ui_tui/tui_app.cpp` com motor TUI MS-DOS monocromático ZX-81, suporte a duplo buffer, menus suspensos, desktop quadriculado, barra de status e caixa de diálogo modal de About.
  4. Atualização de `src/main.cpp`, `CMakeLists.txt` e `include/core/version.h` para a versão `v0.1.1` (`Sinclair ZX81 (Foundation)` / `3D Monster Maze` / `CLI interativo e modo TUI retro estilo MS-DOS`).
  5. Atualização de `CHANGELOG.md`, `RELEASE.md`, `MANUAL.md` e `OUTLINE.md`.
  6. Compilação e empacotamento completo via `build.ps1`.
  7. Refinamento visual da TUI: padronização da cor de fundo do desktop para ser homogênea e uniforme em toda a extensão, eliminando o padrão xadrez claro/escuro e mantendo o caractere quadriculado `░` em cinza escuro sobre fundo preto uniforme. Versão incrementada para `v0.1.2` (`Fundo uniforme com quadriculado no desktop TUI`).
  8. Correção das bordas UTF-8 no TUI: resolvido o bug em `ScreenBuffer::put_string` que iterava byte a byte ao invés de codepoint UTF-8, dividindo caracteres de moldura (`┌`, `─`, etc.) em fragmentos inválidos que causavam losangos com interrogação em Nerd Fonts (JetBrains Mono). Implementado decodificador UTF-8 nativo, `repeat_utf8` e `SetConsoleCP(CP_UTF8)`. Versão incrementada para `v0.1.3` (`Correcao no parse UTF-8 das bordas no TUI`).
  9. Alinhamento e ampliação dos menus suspensos: ampliada a largura das caixas dropdown para 22 colunas e implementado o formatador `format_menu_item`, garantindo que os rótulos e atalhos fiquem contidos exatamente na área útil interna (`inner_w = 20`) sem encostar ou sobrepor a borda vertical direita (`│`). Versão incrementada para `v0.1.4` (`Alinhamento e ampliacao da borda dos menus TUI`).

### Sessão 06 - 16/10/2026
* **Ambiente:** Windows 11, Antigravity IDE (DeepMind), Modelo: Google Gemini.
* **Solicitação do Usuário:**
  * Usar o SQLite como banco de dados para os parâmetros e configurações dos programas (`zx81ide.db`).
  * Criar um sistema de projetos (`.zx81project`), onde cada arquivo é um banco SQLite autocontido reunindo configurações do projeto, códigos-fonte de usuários em BASIC e ASM, telas, compilados binários, manuais, etc.
  * Criar imediatamente uma configuração inicial que solicita o tema a ser usado no `zx81ide` (`normal`, `invertido`, `claro`, `escuro`), preparando o terreno para os parâmetros visuais de cada tema.
* **Ações Executadas:**
  1. Integração da biblioteca oficial SQLite 3 Amalgamation em `third_party/sqlite3/` (`sqlite3.c`, `sqlite3.h`), com compilação estática transparente e sem dependências de DLLs.
  2. Implementação da camada C++ RAII em `include/core/db.h` e `src/core/db.cpp` (`Database`, `Statement`) com suporte a binds, transações ACID e consultas seguras.
  3. Implementação do `ConfigManager` em `include/core/config_manager.h` e `src/core/config_manager.cpp`, persistindo preferências na tabela `settings` do SQLite (`zx81ide.db`).
  4. Implementação do diálogo interativo na inicialização que detecta ausência de tema e solicita a escolha (`[1] normal`, `[2] invertido`, `[3] claro`, `[4] escuro`).
  5. Implementação do sistema de projetos em `include/core/project.h` e `src/core/project.cpp`, inicializando schemas para metadados, fontes (`sources`), telas (`screens`), binários (`binaries`), manuais (`manuals`) e recursos (`assets`), além de semear `main.bas` e `MANUAL.md` na criação de novos projetos.
  6. Adição de comandos CLI completos para temas, configurações e projetos (`theme`, `config [list|get|set]`, `project [new|info|list|add-source|view-source|export-source|add-doc|view-doc]`).
  7. Atualização do modo TUI para exibir dinamicamente o tema na barra de status e no diálogo Sobre.
  8. Atualização de `build.ps1`, `CMakeLists.txt`, `CHANGELOG.md`, `RELEASE.md`, `README.md` e `MANUAL.md`.
  9. Compilação bem-sucedida, testes funcionais de CLI/SQLite e geração do pacote `zx81ide-v-0.1.5.zip`.

### Sessão 07 - 16/10/2026
* **Ambiente:** Windows 11, Antigravity IDE (DeepMind), Modelo: Google Gemini.
* **Solicitação do Usuário:**
  * Implementação visual das especificações do tema invertido:
    * Menus com fundo cinza e letras pretas.
    * Destaques e seleção com fundo preto e letras brancas.
    * Desktop invertendo as cores do normal (`FG_BLACK` em `BG_GRAY_DARK`), mantendo a textura harmônica.
    * Barra de status com fundo preto e letras brancas.
* **Ações Executadas:**
  1. Criação da estrutura de dados `TuiTheme` e da rotina `get_tui_theme(name)` em `src/ui_tui/tui_app.cpp` desacoplando as cores do motor de renderização.
  2. Implementação das regras do tema invertido em todos os elementos da TUI (barra de menus, caixas suspensas, destaques de seleção, desktop, barra de status e diálogo Sobre).
  3. Adição das teclas de atalho `T` e `F2` na interface TUI para alternância dinâmica de temas com persistência automática no SQLite (`zx81ide.db`).
  4. Incremento para versão `v0.1.6` (`Implementacao visual do tema invertido no modo TUI`).
  5. Atualização dos documentos vivos (`CMakeLists.txt`, `CHANGELOG.md`, `RELEASE.md`, `README.md`, `MANUAL.md` e `OUTLINE.md`).
  6. Compilação, testes funcionais e geração do pacote de distribuição `zx81ide-v-0.1.6.zip`.

### Sessão 08 - 16/10/2026
* **Ambiente:** Windows 11, Antigravity IDE (DeepMind), Modelo: Google Gemini.
* **Solicitação do Usuário:**
  * Implementação visual das especificações dos temas escuro e claro:
    * **Tema Escuro:** Desktop com cores preta e cinza escuro, barra de menus em preto com letras brancas, destaques em branco com letras pretas, barra de status com fundo preto e letras brancas.
    * **Tema Claro:** Desktop com letra e fundo cinzas, barra de status com fundo branco e letras pretas, barra de menus com fundo branco e letras pretas, destaques com fundo cinza e letras brancas.
* **Ações Executadas:**
  1. Atualização do método `get_tui_theme()` em `src/ui_tui/tui_app.cpp` calibrando os contrastes para os temas `escuro` e `claro`, cobrindo desktop, menus, dropdowns, barra de status e diálogos modais.
  2. Incremento de versão para `v0.1.7` (`Implementacao visual dos temas escuro e claro no modo TUI`).
  3. Atualização dos documentos vivos (`include/core/version.h`, `CMakeLists.txt`, `CHANGELOG.md`, `RELEASE.md`, `README.md`, `MANUAL.md` e `OUTLINE.md`).
  4. Compilação limpa via `build.ps1 -Clean`, testes funcionais e geração do pacote `zx81ide-v-0.1.7.zip`.
  5. Commit, tag e publicação do release no GitHub via `gh release create`.

### Sessão 09 - 16/10/2026
* **Ambiente:** Windows 11, Antigravity IDE (DeepMind), Modelo: Google Gemini.
* **Solicitação do Usuário:**
  * Criação do editor de código do zx81ide:
    * Definição da arquitetura dos editores: 2 apresentações (CLI e TUI) x 2 estilos (Livre moderno padrão Turbo C++ / Turbo Pascal 6 e Classic fiel ao Sinclair ZX-81), totalizando 4 combinações.
    * Implementação inicial: Editor Livre Simples para o modo CLI (`editor <arquivo.bas>`), abrindo janela padrão Turbo C++ com moldura dupla IBM-PC e 80x25 caracteres úteis para escrita.
    * Barra de título com botão fechar mouseável `[■]`, nome do arquivo e botão maximizar/restaurar (`=▲=` / `=↕=`).
    * Barra de rolagem vertical à direita e horizontal inferior, com caractere de junção de quadro duplo `╬` no canto inferior direito para redimensionamento.
    * Suporte a inserção de texto, `Enter`, `Backspace`, `Delete`, setas de navegação, atalhos de salvar (`F2`/`Ctrl+S`) e diálogo de saída com confirmação.
* **Ações Executadas:**
  1. Criação de `include/editor/text_editor.h` e `src/editor/text_editor.cpp` com suporte a teclado e mouse (clique, arrasto, roda de rolagem e redimensionamento).
  2. Integração do comando `editor <arquivo>` no console interativo CLI e como argumento de linha de comando (`zx81ide editor <arquivo>` e `zx81ide <arquivo>`).
  3. Atualização de `CMakeLists.txt`, `include/core/version.h` (versão `v0.1.8`), `CHANGELOG.md`, `RELEASE.md`, `README.md`, `MANUAL.md` e `OUTLINE.md`.
  4. Compilação via `build.ps1 -Clean`, geração do pacote compactado `zx81ide-v-0.1.8.zip`.
  5. Commit, tag e publicação do release no GitHub via `gh release create`.

---

## 4. Guia Rápido para Próxima IA ou Novo PC

Se você for uma nova instância de IA ou se o desenvolvedor estiver em um novo ambiente, siga estes passos para retomar imediatamente o trabalho:

1. **Verificar o estado do repositório:**
   ```bash
   git status
   git log -n 5 --oneline
   ```
2. **Consultar os arquivos norteadores:**
   * `SPEC.md`: Especificações das próximas features a serem implementadas.
   * `OUTLINE.md`: Este arquivo, para entender o histórico e decisões de design.
   * `CHANGELOG.md` e `RELEASE.md`: Para conferir a numeração atual e regras de versionamento.
3. **Verificar os compiladores disponíveis:**
   ```powershell
   cmake --version
   g++ --version
   clang++ --version
   ```
4. **Executar o build de teste:**
   ```powershell
   .\build.ps1
   ```
5. **Próximas Tarefas Prioritárias:**
   * Implementação do parser de linha de comando CLI (`include/cli/` e `src/cli/`).
   * Integração inicial do conversor BASIC / gerador de snapshots `.p` (`include/compiler/` e `src/compiler/`).
   * Adição dos submódulos em `third_party/` (SJAsmPlus, EightyOne core).
