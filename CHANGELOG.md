# Changelog

Todas as alterações notáveis neste projeto serão documentadas neste arquivo.

O formato é baseado em [Keep a Changelog](https://keepachangelog.com/pt-BR/1.0.0/), e este projeto adere ao esquema de versionamento personalizado:
* **`X` (Major):** Nome e modelo clássico de ZX-81 e clones (ex: *Sinclair ZX81*, *Timex Sinclair 1000*, *Microdigital TK85*).
* **`Y` (Minor):** Nome de jogo clássico de ZX-81 (ex: *3D Monster Maze*, *Mazogs*, *Flight Simulation*).
* **`Z` (Patch/Build):** Subtítulo curto indicando o que foi alterado.

---

## [0.1.13] - 2026-10-17
### Codename: "3D Monster Maze"
### Subtítulo: "Suporte a mouse aprimorado com transicao de clique, cursor de bloco quadrado MS-DOS e arrasto protegido"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Corrigido (Fixed)
* **Eliminação de Loops de Redesenho no Menu:**
  * O processamento de eventos de mouse agora rastreia a transição de estado do botão esquerdo (`prev_btn_state`), garantindo que um clique em opções do menu ou na barra superior seja disparado estritamente **uma única vez** no momento em que o botão é pressionado (`left_clicked`), e não repetidamente em cada micro-movimento do mouse enquanto o botão estiver pressionado.
* **Arrasto Seguro da Janela sem Fechamentos ou Zoom Acidentais:**
  * O arrasto da janela flutuante pelo cabeçalho superior (`is_dragging`) agora é estritamente isolado: cliques de arrasto só iniciam na área central do título (entre os botões `[×]` e `[▲]`).
  * Enquanto a janela estiver sendo arrastada (`is_dragging == true`), todas as verificações de clique em botões da janela são desativadas, impedindo que o movimento do cursor sobre as antigas coordenadas do botão fechar (`[×]`) encerre a janela.
  * O botão de maximizar/restaurar (`[▲]` / `[↕]`) agora exige clique explícito (`left_clicked`), não disparando mais por simples passagem do cursor (hover).
* **Hover Suave nos Menus:**
  * Quando um menu suspenso estiver aberto, o movimento do cursor sobre a linha 0 alterna os menus suavemente, e o movimento sobre a lista de opções destaca o item sob o ponteiro sem disparar ações acidentais.

### Adicionado (Added)
* **Cursor Quadrado Bloco Estilo MS-DOS Clássico:**
  * Configurado o cursor do console no editor e na TUI como um **bloco sólido quadrado** (100% da altura da célula) tanto via API nativa do Windows (`CONSOLE_CURSOR_INFO` com `dwSize = 100`) quanto através da sequência de escape ANSI VT (`\x1b[1 q` - blinking block cursor), recriando com perfeição visual a experiência clássica de edição do MS-DOS (Turbo C++ / Turbo Pascal / MS-DOS Edit).
  * O cursor original do terminal do usuário é preservado e restaurado com exatidão ao sair (`\x1b[0 q` e restauração da estrutura do console).

---

## [0.1.12] - 2026-10-17
### Codename: "3D Monster Maze"
### Subtítulo: "Editor Livre integrado na TUI com janela flutuante, redimensionavel e com sombra MS-DOS"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Adicionado (Added)
* **Editor Livre Integrado como Janela Flutuante na TUI (`src/editor/text_editor.cpp`, `src/ui_tui/tui_app.cpp`):**
  * Em modo TUI (`EditorPresentation::TUI_INTEGRATED` / `zx81ide --tui`), o editor abre como uma janela flutuante centralizada sobre o desktop texturizado (`░░`), preservando margens visíveis em todas as quatro direções em vez de preencher 100% da tela.
  * Efeito autêntico de sombra projetada (drop shadow) estilo MS-DOS Turbo Vision (`▒▒`) projetado à direita e abaixo da janela sobre o fundo quadriculado do desktop.
  * Redimensionamento interativo com mouse pelo puxador inferior direito (`◢`), arrasto suave segurando a barra de título superior com o botão esquerdo e suporte a zoom/maximizar (`[▲]` / `[↕]` ou `F5`) alternando perfeitamente entre tamanho flutuante e tela cheia.
  * Suporte a múltiplos códigos de saída (`return exit_return_code;`):
    * Fechar a janela (`[×]`, `Ctrl+F4`, `Esc` ou menu `Arquivo -> Fechar Janela`) encerra a edição e retorna ao desktop da TUI sem finalizar o executável.
    * Sair da TUI (`Alt+X`, `Ctrl+Q` ou menu `Arquivo -> Sair da TUI`) encerra tanto o editor quanto a TUI, retornando diretamente ao console do sistema operacional.
    * Diálogo modal de confirmação de alterações adaptado dinamicamente: pergunta *"Fechar Janela do Editor"* ao fechar a janela ou *"Salvar e Sair"* ao sair da TUI.
* **Suporte Completo a Mouse na TUI Desktop (`src/ui_tui/tui_app.cpp`):**
  * Atualizado o loop principal de eventos da TUI para `ReadConsoleInputW` com modo de console avançado (`ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_EXTENDED_FLAGS`).
  * Interação via clique de mouse na barra de menus superior (`Arquivo`, `Opcoes`, `Ajuda`), nos itens dos menus suspensos (dropdowns) e no botão `[ OK ]` da caixa de diálogo *Sobre*.
  * Suporte a argumento opcional de arquivo no comando `zx81ide --tui <arquivo.bas>` e no comando interativo `tui <arquivo.bas>`, abrindo o editor flutuante imediatamente sobre o desktop.

---

## [0.1.11] - 2026-10-17
### Codename: "3D Monster Maze"
### Subtítulo: "Menu bar persistente no editor, correcao do atalho F9 e compilacao automatica ZXText2P"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Corrigido (Fixed)
* **Barra Superior de Menus Persistente no Editor (`src/editor/text_editor.cpp`):**
  * A barra de menus principal (`Arquivo`, `Editar`, `Compilar`, `Opcoes`, `Ajuda`) agora permanece visível e permanentemente ativa na Linha 0 da tela durante toda a execução do editor, tanto na TUI integrada quanto no modo CLI avulso (`zx81ide editor <arquivo>`).
  * A área útil de edição e borda da janela foi delimitada entre a Linha 1 e `term_h - 2`, preservando a Linha 0 para os menus e a Linha `term_h - 1` para a barra de status e atalhos rápidos.
  * Menus suspensos (dropdowns) com bordas `┌─┐│└─┘` e sombras de bloco pontilhado estilo MS-DOS (`▒`), com suporte a teclado (`F10`, `Alt+A/E/C/O/H`, setas direcionais, `Enter`) e clique de mouse.
* **Correção do Atalho de Compilação `F9` e Feedback de Modais:**
  * Corrigido encerramento prematuro e invisível dos modais de sucesso e erro: substituída a instrução `continue` por `break` no loop de eventos do console (`ReadConsoleInputW`), impedindo que eventos em lote fechem o modal antes da renderização do frame.
  * O modal de compilação com sucesso (`╔═════════[ Compilacao Concluida (.P) ]═════════╗`) e o de erro agora são exibidos de forma estável na tela e aguardam confirmação explícita (`Enter`, `Esc`, `Espaço` ou clique de mouse).
  * Auto-detecção de modo labels/não-numerado no compilador ZXText2P: códigos BASIC sem número de linha inicial ou misturando labels e números explícitos agora compilam com numeração automática em vez de falharem silenciosamente.
  * Garantida a gravação do arquivo `.P` em maiúsculo (ex: `NONAME.P`) no disco para compatibilidade com emuladores (EightyOne) e gravadores de fita.
* **Comando `compile` e `compile-chars` diretamente na Linha de Comando do Sistema Operacional:**
  * O executável agora aceita `zx81ide compile <arquivo.bas> [-o <saida.p>]` diretamente no terminal sem abrir o editor ou requerer o prompt interativo.

---

## [0.1.10] - 2026-10-17
### Codename: "3D Monster Maze"
### Subtítulo: "Compilador ZXText2P C++ nativo, syntax highlighting e seletor de caracteres graficos"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Adicionado (Added)
* **Compilador ZXText2P Nativo em C++ (`include/compiler/zxtext2p.h`, `src/compiler/zxtext2p.cpp`):**
  * Portabilidade e incorporação total do algoritmo ZXText2P em C++ moderno no próprio executável `zx81ide`, sem dependência de binários externos.
  * Suporte a programas em Sinclair BASIC com números de linha explícitos (`10 PRINT "OI"`) e programas modulares estruturados com labels (`@loop: ... GOTO @loop`, estilo Dignified Suite).
  * Conversor e gerador estrito de binários `.P` do Sinclair ZX-81, gerando:
    * As 116 System Variables originais a partir de `0x4009` (endereços de `D_FILE`, `VARS`, `E_LINE`, `CH_ADD`, `STKBOT`, flags PAL/NTSC, etc.).
    * Linhas de programa tokenizadas com identificador big-endian, comprimento little-endian e delimitador `0x76` (NEWLINE).
    * Suporte e codificação inline de ponto flutuante de 5 bytes do ZX-81 (marcador `0x7E` + expoente e mantissa) para todas as constantes numéricas.
    * Geração do Display File (`D_FILE` com 24 linhas de 32 espaços e terminadores `0x76`) e bloco de variáveis (`VARS` com `0x80, 0x76`).
    * Tratamento completo de caracteres de vídeo inverso (`%X`), aspas escapadas (`\"`), símbolos pound (`\#`), e caracteres de blocos gráficos (`\xy`).
    * Validação com SHA-256 idêntico byte a byte aos arquivos gerados pelo utilitário oficial `zxtext2p.exe`.
* **Destaque de Sintaxe ZX-81 (Syntax Highlighting) no Editor:**
  * Realce léxico em tempo real integrado à janela do editor, adaptado à paleta monocromática estrita do ZX-81:
    * **Palavras-chave e Tokens:** `PRINT`, `LET`, `IF`, `THEN`, `FOR`, `TO`, `STEP`, `NEXT`, `GOTO`, `GOSUB`, `REM`, `CLS`, `INPUT`, `PAUSE`, `PLOT`, etc.
    * **Comentários:** `#` (estilo zxtext2p) e comandos `REM` destacados com atenuação visual.
    * **Strings:** Textos literais entre aspas com identificação de escapes internos.
    * **Números e Linhas:** Literais numéricos e números de instrução destacados.
    * **Labels:** Definições `@label:` e chamadas `@label` identificadas com destaque.
    * **Caracteres Gráficos e Inversos:** Escapes `\xy`, `\::`, `\:'`, `%X` exibidos com contraste especial.
    * Garantia estrita de renderização de colunas (largura fixa `inner_w`), mantendo o alinhamento da janela 100% perfeito.
* **Seletor Visual de Caracteres Gráficos do ZX-81 (`F4` / `Ctrl+G`):**
  * Diálogo modal interativo em grade exibindo os 24 blocos gráficos e escapes do Sinclair ZX-81.
  * Visualização gráfica em UTF-8 (`▘`, `▝`, `▀`, `▖`, `▌`, `▞`, `▛`, `█`, `░`, `▒`, `▓`, etc.), código de escape correspondente (`\:'`, `\::`, `\' `, etc.), descrição em português e código interno do ZX-81.
  * Navegação por setas (`Cima`, `Baixo`, `Esquerda`, `Direita`) ou mouse e tecla `Enter` para inserção direta no cursor do editor.
* **Compilação e Salvamento Direto no Editor (`F9` / `Ctrl+B`):**
  * Atalho `F9` compila instantaneamente o código aberto para o formato binário `.P`.
  * Em caso de sucesso, grava o arquivo `.P` e exibe relatório de bytes e linhas.
  * Em caso de erro de sintaxe, exibe diálogo modal com a linha exata e a descrição da falha, posicionando o cursor automaticamente na linha problemática.
* **Modal de Parâmetros do Compilador (`F7` / `Ctrl+T`):**
  * Configuração interativa do modo de labels (`-l`), linha inicial (`-s`) e incremento (`-i`), persistida no banco SQLite `zx81ide.db`.
* **Comandos no Console CLI:**
  * Comando `compile <arquivo.bas> [-o <saida.p>] [-l] [-s <inicio>] [-i <incremento>]`.
  * Comando `compile-chars` para exibição rápida da tabela de caracteres gráficos e seus escapes no console.
* **Integração na TUI Retro:**
  * Submenu Arquivo atualizado com opção `Editor` (`F3`) e atalhos globais.

---

## [0.1.9] - 2026-10-16
### Codename: "3D Monster Maze"
### Subtítulo: "Ajuste de alinhamento e novos icones de fechar e redimensionar no editor"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Corrigido (Fixed)
* **Alinhamento e Extrapolação da Área de Edição:**
  * Corrigida a divergência de 2 colunas entre a barra de título superior e o corpo do editor. A barra superior reservava 14 caracteres fixos em vez dos 12 estritamente ocupados (`╔`, `═[×]═`, `═[▲]═`, `╗`), fazendo a moldura superior ser renderizada 2 colunas mais curta que a área útil de texto e a moldura inferior.
  * Implementada a rotina `format_editor_line()` com expansão automática de caracteres de tabulação (`\t` para 4 espaços) e garantia de largura exata de `inner_w` colunas por linha de texto, eliminando qualquer salto ou extrapolação à direita.

### Modificado (Changed)
* **Novo Ícone de Fechamento de Janela (`[×]`):** Substituição do símbolo de quadrado `[■]` pelo clássico `[×]` (multiplication cross U+00D7), muito mais elegante, legível e padronizado em fontes monoespaçadas (como JetBrains Nerd Font).
* **Novo Símbolo de Redimensionamento da Janela (`◢`):** Substituição da junção em cruz dupla `╬` pelo grip triangular de canto inferior direito `◢` (U+25E2), oferecendo indicação visual imediata de alça de redimensionamento e harmonizando perfeitamente com a ponta da barra de rolagem vertical `▼`.

---

## [0.1.8] - 2026-10-16
### Codename: "3D Monster Maze"
### Subtítulo: "Implementacao do Editor de Texto Livre Turbo C++ no modo CLI"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Adicionado (Added)
* **Módulo do Editor de Texto Livre Turbo C++ (`editor/text_editor.h`, `src/editor/text_editor.cpp`):**
  * Primeira das 4 combinações de editores planejadas (Estilo Livre em Apresentação Standalone na CLI).
  * **Janela Estilo Turbo C++ / Turbo Pascal 6:** Moldura dupla IBM-PC (`╔`, `═`, `╗`, `║`, `╚`, `╝`, `╬`) abrindo inicialmente com 80x25 caracteres úteis de escrita (área total 82x27 adaptável ao console).
  * **Barra de Título Funcional:** Botão fechar `[■]` com suporte a clique de mouse, título centralizado com o nome do arquivo e indicador de modificação (`*`), e botão maximizar/restaurar (`=▲=` / `=↕=`).
  * **Barras de Rolagem Bidirecionais:** Barra de rolagem vertical à direita (`▲`, trilho `░`, thumb `█`, `▼`) e barra horizontal inferior (`◄`, trilho `░`, thumb `█`, `►`).
  * **Junção de Redimensionamento:** Caractere de junção de quadro duplo `╬` no canto inferior direito, permitindo redimensionamento dinâmico via clique e arrasto do mouse.
  * **Suporte Completo a Teclado e Mouse:** Inserção de texto, quebra de linha com `Enter`, `Backspace`, `Delete`, `Tab` (4 espaços), navegação por setas (`Cima`, `Baixo`, `Esquerda`, `Direita`), `Home`, `End`, `Page Up`, `Page Down`, roda do mouse (`Mouse Wheel`) e clique direto no texto para posicionamento do cursor.
  * **Atalhos e Diálogo de Confirmação:** Atalho `F2` / `Ctrl+S` para salvar no disco, `F5` para zoom/maximizar, `Esc` / `Alt+X` / `Ctrl+Q` para sair com diálogo modal de confirmação (`Sim`, `Não`, `Cancelar`) quando o arquivo for modificado.
  * **Integração com Paleta Sinclair ZX-81:** O editor adapta suas cores instantaneamente ao tema ativo (`normal`, `invertido`, `escuro`, `claro`) gravado no SQLite.
* **Comandos CLI e Parser:**
  * Comando `editor <arquivo.bas>` adicionado ao console interativo CLI.
  * Argumento de linha de comando `zx81ide editor <arquivo.bas>` ou `zx81ide <arquivo.bas>` para abertura direta do editor.

---

## [0.1.7] - 2026-10-16
### Codename: "3D Monster Maze"
### Subtítulo: "Implementacao visual dos temas escuro e claro no modo TUI"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Adicionado (Added)
* **Implementação Visual Completa do Tema Escuro:**
  * **Desktop:** Textura ASCII (`░`) em cinza escuro (`FG_GRAY_DARK`) sobre fundo preto puro (`BG_BLACK`), garantindo contraste imersivo.
  * **Barra de Menus e Submenus Dropdown:** Fundo preto (`BG_BLACK`) com textos e bordas em branco puro (`FG_WHITE`).
  * **Destaques e Seleção:** Itens ativos na barra de menus e submenus suspensos com fundo branco puro (`BG_WHITE`) e letras em preto (`FG_BLACK`).
  * **Barra de Status:** Fundo preto (`BG_BLACK`) com texto em branco puro (`FG_WHITE`).
  * **Diálogo Modal Sobre (About):** Janela com bordas e corpo em preto, título e botão `[ OK ]` destacados em branco com texto preto.
* **Implementação Visual Completa do Tema Claro:**
  * **Desktop:** Textura ASCII (`░`) com letras em cinza escuro (`FG_GRAY_DARK`) e fundo cinza claro (`BG_GRAY_LIGHT`), produzindo uma tonalidade cinza contínua.
  * **Barra de Menus e Submenus Dropdown:** Fundo branco puro (`BG_WHITE`) com textos e bordas em preto (`FG_BLACK`).
  * **Destaques e Seleção:** Itens ativos na barra de menus e submenus com fundo cinza escuro (`BG_GRAY_DARK`) e letras em branco (`FG_WHITE`).
  * **Barra de Status:** Fundo branco puro (`BG_WHITE`) com texto em preto (`FG_BLACK`).
  * **Diálogo Modal Sobre (About):** Janela com corpo branco e moldura dupla preta, com destaque de título e botão em cinza escuro com texto branco.

---

## [0.1.6] - 2026-10-16
### Codename: "3D Monster Maze"
### Subtítulo: "Implementacao visual do tema invertido no modo TUI"
### Linhagem de Hardware (X): Sinclair ZX81 (Foundation)

### Adicionado (Added)
* **Motor Dinâmico de Temas no TUI (`TuiTheme`):** Desacoplamento de cores estáticas e introdução da rotina `get_tui_theme(name)` mapeando as paletas dos temas.
* **Implementação Visual do Tema Invertido:**
  * **Barra de Menus e Dropdowns:** Fundo cinza claro (`BG_GRAY_LIGHT`) com letras pretas (`FG_BLACK`).
  * **Destaques e Seleção:** Menus ativos e itens selecionados em submenus com fundo preto (`BG_BLACK`) e letras brancas (`FG_WHITE`).
  * **Desktop com Inversão do Padrão:** Caracteres quadriculados ASCII (`░`) em preto (`FG_BLACK`) sobre fundo cinza escuro (`BG_GRAY_DARK`), invertendo o contraste do tema normal e mantendo a textura harmônica.
  * **Barra de Status:** Fundo preto (`BG_BLACK`) com letras brancas (`FG_WHITE`).
  * **Diálogo Modal Sobre (About):** Moldura e textos em preto sobre corpo cinza, com botão `[ OK ]` em fundo preto com letras brancas.
* **Alternância em Tempo Real:** Teclas de atalho `T` ou `F2` na interface TUI para alternar dinamicamente os temas com gravação automática imediata no banco SQLite (`zx81ide.db`).

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
