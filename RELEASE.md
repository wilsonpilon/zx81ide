# RELEASE NOTES - zx81ide

Arquivo oficial de registro e detalhamento de lançamentos do **zx81ide**.

---

## [v0.1.13] - 2026-10-17
### Título do Lançamento: `zx81ide v0.1.13 [3D Monster Maze] - Suporte a mouse aprimorado com transicao de clique, cursor de bloco quadrado MS-DOS e arrasto protegido`

* **Versão:** `0.1.13`
* **Tag Git:** `v0.1.13`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans, 1981)*
* **Subtítulo da Compilação (Z):** `Suporte a mouse aprimorado com transicao de clique, cursor de bloco quadrado MS-DOS e arrasto protegido`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.13.zip`

---

### 1. Resumo do Lançamento
A versão **v0.1.13** aprimora profundamente a precisão do suporte a mouse e recria a experiência autêntica de cursor retrô:
1. **Transição de Clique sem Redesenho Contínuo:** Os cliques em menus e diálogos agora rastreiam o estado anterior do botão (`prev_btn_state`), disparando ações estritamente no pressionamento inicial (`left_clicked`). Acabou o problema de menus ficarem redesenhando continuamente ao clicar em opções.
2. **Arrasto Seguro da Janela:** Durante o arrasto da janela flutuante (`is_dragging`), a verificação de botões de controle (`[×]` fechar e `[▲]` zoom) é desativada, impedindo encerramentos acidentais quando o ponteiro passa próximo às coordenadas dos botões.
3. **Proteção contra Acionamento por Hover:** O botão maximizar/restaurar (`[▲]` / `[↕]`) agora exige clique real com o botão esquerdo, nunca mais maximizando apenas por passar o mouse por cima.
4. **Cursor Quadrado Bloco MS-DOS:** O cursor agora é um quadrado sólido (`█`), preenchendo 100% da célula de texto tanto via API Win32 (`CONSOLE_CURSOR_INFO` dwSize = 100) quanto via ANSI VT (`\x1b[1 q`), com restauração transparente ao sair.

---

## [v0.1.12] - 2026-10-17
### Título do Lançamento: `zx81ide v0.1.12 [3D Monster Maze] - Editor Livre integrado na TUI com janela flutuante, redimensionavel e com sombra MS-DOS`

* **Versão:** `0.1.12`
* **Tag Git:** `v0.1.12`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans, 1981)*
* **Subtítulo da Compilação (Z):** `Editor Livre integrado na TUI com janela flutuante, redimensionavel e com sombra MS-DOS`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.12.zip`

---

### 1. Resumo do Lançamento
A versão **v0.1.12** conclui a integração da TUI com o Editor de Texto, trazendo a experiência clássica de janelas móveis da era MS-DOS / Borland Turbo Vision:
1. **Janela Flutuante com Sombra MS-DOS:** Ao abrir o editor na TUI (`zx81ide --tui`), o editor surge como uma janela flutuante centralizada sobre o desktop pontilhado (`░░`), com bordas visíveis ao redor e autêntica sombra projetada (`▒▒`) no canto inferior e direito.
2. **Arrasto, Redimensionamento e Zoom:** A janela pode ser movida pelo topo segurando com o mouse, redimensionada pelo canto inferior direito (`◢`) e maximizada/restaurada via botão `[▲]` / `[↕]` ou atalho `F5`.
3. **Fechamento Limpo da Janela:** Fechar a janela (`[×]`, `Ctrl+F4`, `Esc` ou `Arquivo -> Fechar Janela`) retorna suavemente ao desktop da TUI. O comando `Arquivo -> Sair da TUI` (`Alt+X`) encerra a aplicação.
4. **Suporte Completo a Mouse na TUI:** A área da TUI (barra de menus superior, submenus suspensos e caixas de diálogo) agora responde a cliques de mouse através da API `ReadConsoleInputW`.

---

## [v0.1.11] - 2026-10-17
### Título do Lançamento: `zx81ide v0.1.11 [3D Monster Maze] - Menu bar persistente no editor, correcao do atalho F9 e compilacao automatica ZXText2P`

* **Versão:** `0.1.11`
* **Tag Git:** `v0.1.11`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans, 1981)*
* **Subtítulo da Compilação (Z):** `Menu bar persistente no editor, correcao do atalho F9 e compilacao automatica ZXText2P`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.11.zip`

---

### 1. Resumo do Lançamento
A versão **v0.1.11** aprimora a usabilidade e a estabilidade do fluxo de edição e compilação do **zx81ide**:
1. **Menu Bar Persistente no Editor:** A barra de menus superior (`Arquivo`, `Editar`, `Compilar`, `Opcoes`, `Ajuda`) agora permanece permanentemente visível na Linha 0 em todos os modos e tamanhos de janela, com menus dropdown em cascata contendo sombra pontilhada MS-DOS (`▒`), navegação por teclado (`F10`, `Alt+A/E/C/O/H`) e suporte integral a mouse.
2. **Correção do Atalho F9 e Feedback Visual dos Modais:** O acionamento de `F9` ou `Compilar -> Compilar para .P` agora compila confiavelmente o código fonte para binário `.P` e exibe o modal de sucesso com métricas de bytes e linhas sem fechamento prematuro no loop de eventos do console.
3. **Auto-detecção de Modo Labels no Compilador ZXText2P:** Fontes BASIC sem numeração inicial ou com numeração mista são auto-detectados e compilados com numeração automática em vez de falharem silenciosamente.
4. **Execução de Compilação Direta via CLI:** O comando `zx81ide compile <arquivo.bas> [-o <saida.p>]` agora pode ser executado diretamente do terminal do sistema operacional (PowerShell / Prompt de Comando).

---

## [v0.1.10] - 2026-10-17
### Título do Lançamento: `zx81ide v0.1.10 [3D Monster Maze] - Compilador ZXText2P C++ nativo, syntax highlighting e seletor de caracteres graficos`

* **Versão:** `0.1.10`
* **Tag Git:** `v0.1.10`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans, 1981)*
* **Subtítulo da Compilação (Z):** `Compilador ZXText2P C++ nativo, syntax highlighting e seletor de caracteres graficos`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.10.zip`

---

### 1. Resumo do Lançamento
A versão **v0.1.10** atinge um marco de maturidade técnica fundamental no projeto **zx81ide**, integrando de forma 100% nativa em C++ o algoritmo de compilação e tokenização do **ZXText2P** (dispensando qualquer binário externo), adicionando realce de sintaxe em tempo real (Syntax Highlighting) para Sinclair BASIC e formato de labels no editor de texto, e introduzindo uma central de caracteres gráficos e de bloco do ZX-81 acessível por atalho modal (`F4` / `Ctrl+G`).

Além disso, o fluxo de desenvolvimento agora permite compilar e salvar arquivos `.P` diretamente do editor (`F9`), salvando o código ASCII `.BAS` (`F2`), com feedback imediato de sucesso ou diagnóstico de erros de compilação com pulo automático para a linha da falha.

---

### 2. O Que Foi Entregue Nesta Versão
* **Compilador ZXText2P C++ Nativo Incorporado:**
  * Reimplementação completa em C++ moderno (`include/compiler/zxtext2p.h`, `src/compiler/zxtext2p.cpp`) baseada na especificação do utilitário `zxtext2p`.
  * Geração do formato binário `.P` com exatidão byte-a-byte (validado via hash criptográfico SHA-256 idêntico ao gerado pelo binário original de referência).
  * Suporte a 100% dos recursos do ZXText2P:
    * Modo tradicional numerado e modo estruturado com labels (`@loop: ... GOTO @loop`, estilo Dignified Suite).
    * Parâmetros configuráveis de linha inicial (`-s`) e incremento de linhas (`-i`).
    * Continuação de linha com barra invertida (`\`).
    * Comentários com `#` e ignoramento de linhas em branco.
    * Video inverso prefixado com `%` (`%X` = char code | 128).
    * Blocos gráficos via escape `\xy` (`\:'`, `\::`, `\' `, `\##`, etc.) e aspas escapadas `\"`.
    * Codificação matemática em ponto flutuante de 5 bytes do ZX-81 para todas as constantes numéricas.
    * Geração das 116 System Variables (`0x4009`), Display File (`D_FILE`) e bloco `VARS`.
* **Destaque de Sintaxe (Syntax Highlighting) no Editor:**
  * Analisador léxico integrado que colore e destaca em tempo real:
    * Palavras-chave do Sinclair ZX-81 (em negrito de alto contraste).
    * Comentários `#` e comandos `REM` (em tom suave atenuado).
    * Strings e textos entre aspas duplas.
    * Constantes numéricas e números de linha.
    * Definições e chamadas de labels (`@loop`).
    * Caracteres gráficos e escapes do ZX-81 (com destaque em bloco invertido).
  * Renderização estrita em colunas que preserva rigorosamente a geometria da janela sem extrapolação.
* **Seletor Visual de Caracteres Gráficos (`F4` / `Ctrl+G`):**
  * Diálogo modal interativo em grade apresentando os 24 caracteres gráficos e blocos do ZX-81.
  * Exibição visual com glifos Unicode aproximados, código de escape do ZXText2P, descrição e código interno do Sinclair ZX-81.
  * Inserção com tecla `Enter` ou clique do mouse diretamente na posição do cursor do editor.
* **Compilação e Salvamento Direto no Editor (`F9` / `Ctrl+B`):**
  * Compilação instantânea do código em edição para binário `.P`.
  * Diálogo modal de relatório em caso de sucesso com contagem de bytes e linhas.
  * Diálogo de diagnóstico em caso de erro, com movimentação automática do cursor para a linha com defeito.
* **Comandos no Console CLI:**
  * Comando `compile <arquivo.bas> [-o <saida.p>] [-l] [-s <inicio>] [-i <incremento>]`.
  * Comando `compile-chars` listando a tabela de blocos gráficos no terminal.
* **Configurações Persistidas em SQLite:**
  * Configurações `compiler.labels_mode`, `compiler.start_line` e `compiler.line_increment` armazenadas no banco `zx81ide.db`.

---

## [v0.1.9] - 2026-10-16
### Título do Lançamento: `zx81ide v0.1.9 [3D Monster Maze] - Ajuste de alinhamento e novos icones de fechar e redimensionar no editor`

* **Versão:** `0.1.9`
* **Tag Git:** `v0.1.9`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans, 1981)*
* **Subtítulo da Compilação (Z):** `Ajuste de alinhamento e novos icones de fechar e redimensionar no editor`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.9.zip`

---

### 1. Resumo do Lançamento
A versão **v0.1.9** traz melhorias pontuais de alinhamento e refinamento estético do subsistema do Editor de Texto Livre Turbo C++ no console CLI. A moldura superior foi milimetricamente alinhada com a largura total da janela (sanando a diferença de 2 colunas que provocava a extrapolação do corpo de texto), e foram adotados símbolos muito mais ergonômicos e compatíveis com fontes modernas (como JetBrains Nerd Font) para as ações de fechamento (`[×]`) e redimensionamento (`◢`).

---

### 2. O Que Foi Entregue Nesta Versão
* **Alinhamento Perfeito da Janela:**
  * Correção do cálculo de caracteres reservados na linha 0 (borda superior / título), passando a reservar exatamente 12 colunas (`╔` + `═[×]═` + `═[▲]═` + `╗`). Anteriormente, o cálculo reservava 14 colunas, resultando em uma barra de título 2 colunas mais estreita que a área de edição.
  * Formatação uniforme de linhas via `format_editor_line()`, garantindo expansão de `\t` para 4 espaços e extensão estrita de `inner_w` colunas visíveis por linha.
* **Novos Símbolos de Interface:**
  * **Botão Fechar (`[×]`):** Substituição do bloco quadrado `[■]` pelo clássico `[×]` (multiplication cross U+00D7), amplamente reconhecido, balanceado e estável em todos os emuladores de terminal.
  * **Alça de Redimensionamento (`◢`):** Substituição da cruz dupla `╬` pelo grip triangular `◢` (U+25E2) no canto inferior direito, harmonizando perfeitamente com a seta inferior da barra de rolagem `▼` e provendo clara indicação tátil/visual para arrasto com o mouse.
* **Pacote de Distribuição:** Binário compilado `zx81ide.exe` e pacote compactado `zx81ide-v-0.1.9.zip`.

---

## [v0.1.8] - 2026-10-16
### Título do Lançamento: `zx81ide v0.1.8 [3D Monster Maze] - Implementacao do Editor de Texto Livre Turbo C++ no modo CLI`

* **Versão:** `0.1.8`
* **Tag Git:** `v0.1.8`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans, 1981)*
* **Subtítulo da Compilação (Z):** `Implementacao do Editor de Texto Livre Turbo C++ no modo CLI`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.8.zip`

---

### 1. Resumo do Lançamento
A versão **v0.1.8** inaugura o subsistema de edição do **zx81ide** entregando o **Editor Livre de Textos Moderno** em modo standalone para console CLI, com estética fiel aos ambientes de desenvolvimento MS-DOS dos anos 1990 (Turbo C++ e Turbo Pascal 6 da Borland). O editor conta com moldura dupla IBM-PC, área útil inicial de 80x25 caracteres, barra de título interativa com botão fechar mouseável `[■]`, botão de zoom/maximização (`=▲=` / `=↕=`), barras de rolagem bidirecionais (vertical e horizontal) com indicadores de posição e junção `╬` para redimensionamento da janela.

---

### 2. O Que Foi Entregue Nesta Versão
* **Subsistema do Editor (`include/editor/text_editor.h`, `src/editor/text_editor.cpp`):**
  * Arquitetura extensível suportando as 4 combinações de editores do projeto (Modos CLI Standalone / TUI Integrado x Estilos Livre Moderno / ZX-81 Clássico).
  * Janela Turbo C++ com moldura dupla IBM-PC (`╔`, `═`, `╗`, `║`, `╚`, `╝`, `╬`), abrindo inicialmente em 80x25 caracteres úteis de escrita.
  * Botão de fechamento por mouse no topo esquerdo (`[■]`), com cálculo dinâmico de coordenadas de clique no console.
  * Título centralizado exibindo o nome do arquivo e indicador de modificação (`*`).
  * Botão de maximização e restauração no canto superior direito (`[▲]` e `[↕]`), alternável por clique ou tecla `F5`.
  * Barra de rolagem vertical à direita (`▲`, `░`, `█`, `▼`) com atualização de thumb proporcional ao número de linhas.
  * Barra de rolagem horizontal inferior (`◄`, `░`, `█`, `►`) com thumb proporcional à largura do texto.
  * Canto inferior direito com caractere de junção `╬` habilitado para redimensionamento da janela via clique e arrasto do mouse.
  * Barra de título com suporte a arrasto para mover a janela pela tela do console.
  * Suporte ergonômico a teclado: inserção de caracteres normais e acentuados, quebra de linha (`Enter`), retrocesso (`Backspace`), exclusão (`Delete`), tabulação (`Tab` com 4 espaços), navegação por setas (`Cima`, `Baixo`, `Esquerda`, `Direita`), `Home`, `End`, `Page Up` e `Page Down`.
  * Suporte a roda do mouse (`Mouse Wheel`) para rolagem vertical rápida do documento.
  * Atalho `F2` e `Ctrl+S` para salvamento imediato do arquivo no disco.
  * Atalho `Esc`, `Alt+X` ou `Ctrl+Q` com diálogo modal de confirmação para salvar alterações pendentes.
  * Calibração automática das cores conforme o tema configurado no SQLite (`normal`, `invertido`, `escuro`, `claro`).
* **Integração no Console CLI e Linha de Comando:**
  * Comando `editor <arquivo.bas>` ou `edit <arquivo.bas>` disponível no console interativo CLI.
  * Suporte a argumentos diretos: `zx81ide editor <arquivo.bas>` ou `zx81ide <arquivo.bas>`.
* **Pacote de Distribuição:** Binário compilado `zx81ide.exe` e pacote compactado `zx81ide-v-0.1.8.zip`.

---

## [v0.1.7] - 2026-10-16
### Título do Lançamento: `zx81ide v0.1.7 [3D Monster Maze] - Implementacao visual dos temas escuro e claro no modo TUI`

* **Versão:** `0.1.7`
* **Tag Git:** `v0.1.7`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans, 1981)*
* **Subtítulo da Compilação (Z):** `Implementacao visual dos temas escuro e claro no modo TUI`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.7.zip`

---

### 1. Resumo do Lançamento
A versão **v0.1.7** conclui o conjunto visual dos 4 temas do **zx81ide** no modo TUI, trazendo a implementação visual estrita dos temas **Escuro** e **Claro**. O sistema mantém a fidelidade estética ao universo do Sinclair ZX-81 utilizando exclusivamente variações de **Preto**, **Branco** e **Cinza**, com contrastes calibrados para legibilidade e ergonomia visual.

---

### 2. O Que Foi Entregue Nesta Versão
* **Tema Escuro (`escuro`):**
  * **Desktop:** Textura ASCII (`░`) em cinza escuro (`FG_GRAY_DARK`) sobre fundo preto puro (`BG_BLACK`), oferecendo visual escuro elegante.
  * **Barra de Menus e Dropdowns:** Fundo preto (`BG_BLACK`) com textos e molduras em branco puro (`FG_WHITE`).
  * **Destaques e Seleção:** Menus ativos e opções de submenus com fundo branco puro (`BG_WHITE`) e letras em preto (`FG_BLACK`).
  * **Barra de Status:** Fundo preto (`BG_BLACK`) com letras em branco puro (`FG_WHITE`).
  * **Diálogo Modal Sobre (About):** Janela com bordas e corpo em preto, título e botão `[ OK ]` com destaque em branco e letras pretas.
* **Tema Claro (`claro`):**
  * **Desktop:** Textura ASCII (`░`) em cinza escuro (`FG_GRAY_DARK`) sobre fundo cinza claro (`BG_GRAY_LIGHT`), gerando uma superfície cinza contínua e suave.
  * **Barra de Menus e Dropdowns:** Fundo branco puro (`BG_WHITE`) com textos e molduras em preto (`FG_BLACK`).
  * **Destaques e Seleção:** Menus ativos e opções selecionadas com fundo cinza escuro (`BG_GRAY_DARK`) e letras em branco puro (`FG_WHITE`).
  * **Barra de Status:** Fundo branco puro (`BG_WHITE`) com letras em preto (`FG_BLACK`).
  * **Diálogo Modal Sobre (About):** Janela com corpo branco puro, moldura dupla em preto e destaque de título e botão em cinza escuro com texto branco.
* **Pacote de Distribuição:** Binário compilado e pacote compactado `zx81ide-v-0.1.7.zip`.

---

## [v0.1.6] - 2026-10-16
### Título do Lançamento: `zx81ide v0.1.6 [3D Monster Maze] - Implementacao visual do tema invertido no modo TUI`

* **Versão:** `0.1.6`
* **Tag Git:** `v0.1.6`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans, 1981)*
* **Subtítulo da Compilação (Z):** `Implementacao visual do tema invertido no modo TUI`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.6.zip`

---

### 1. Resumo do Lançamento
A versão **v0.1.6** entrega a implementação visual completa do **Tema Invertido** no modo TUI do `zx81ide`, com inversão proporcional de contraste e paleta estrita Sinclair ZX-81 (preto, branco e cinza), permitindo alternância instantânea tanto via CLI quanto por tecla de atalho em tempo real no console gráfico.

---

### 2. O Que Foi Entregue Nesta Versão
* **Paleta Invertida Fiel às Especificações:**
  * **Barra de Menus e Caixas de Dropdown:** Fundo cinza claro (`BG_GRAY_LIGHT`) com textos e bordas em preto (`FG_BLACK`).
  * **Destaques e Seleção:** Itens ativos em menus e dropdowns com fundo preto (`BG_BLACK`) e letras em branco puro (`FG_WHITE`).
  * **Desktop Invertido:** Caracteres quadriculados ASCII (`░`) em preto (`FG_BLACK`) sobre fundo cinza escuro uniforme (`BG_GRAY_DARK`), invertendo a polaridade do tema normal com harmonia visual.
  * **Barra de Status:** Fundo preto invertido (`BG_BLACK`) com caracteres brancos (`FG_WHITE`).
  * **Diálogo Modal Sobre (About):** Janela com corpo cinza claro, moldura dupla em preto e botão `[ OK ]` com destaque preto e letras brancas.
* **Alternância Interativa de Temas:** Pressionar `T` ou `F2` na interface TUI alterna os temas em tempo de execução e salva a escolha imediatamente no banco SQLite (`zx81ide.db`).
* **Pacote de Distribuição:** Binário compilado e pacote compactado `zx81ide-v-0.1.6.zip`.

---

## [v0.1.5] - 2026-10-16
### Título do Lançamento: `zx81ide v0.1.5 [3D Monster Maze] - Integracao SQLite, Banco de Configuracoes, Temas e Sistema de Projetos .zx81project`

* **Versão:** `0.1.5`
* **Tag Git:** `v0.1.5`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans, 1981)*
* **Subtítulo da Compilação (Z):** `Integracao SQLite, Banco de Configuracoes, Temas e Sistema de Projetos .zx81project`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.5.zip`

---

### 1. Resumo do Lançamento
A versão **v0.1.5** introduz o motor de persistência relacional baseado em **SQLite 3**, viabilizando dois pilares fundamentais da arquitetura do `zx81ide`:
1. **Banco de Dados de Configurações Global (`zx81ide.db`):** Armazenamento estruturado de preferências e parâmetros da IDE, com assistente de inicialização interativo para escolha imediata do tema (`normal`, `invertido`, `claro`, `escuro`).
2. **Sistema de Projetos Portáteis (`.zx81project`):** Arquivos de projeto que são bancos de dados SQLite autocontidos, unificando códigos-fonte de usuários em BASIC e ASM, telas (DFILE/ASCII), binários compilados (.p), documentação em Markdown e assets.

---

### 2. O Que Foi Entregue Nesta Versão
* **SQLite 3 Amalgamation Integrado:** Inclusão direta de `third_party/sqlite3/sqlite3.c` e `sqlite3.h`, com compilação estática transparente e zero dependência de DLLs externas.
* **Abstração C++ RAII para SQLite:** Classes `Database` e `Statement` em `include/core/db.h` e `src/core/db.cpp`, provendo binds seguros, transações ACID e consultas seguras contra corrupção.
* **Gerenciador de Configurações (`ConfigManager`):** Manipulação de chaves/valores via SQLite com novos comandos CLI: `config [list|get|set]`, `theme [nome]` e `--theme <nome>`.
* **Configuração Inicial de Tema:** Ao iniciar sem configurações salvas, o sistema solicita a seleção do tema desejado (`[1] normal`, `[2] invertido`, `[3] claro`, `[4] escuro`), gravando a preferência no banco SQLite.
* **Manipulador de Projetos (`Project`):** Suporte nativo a `.zx81project` com comandos no console CLI:
  * `project new <proj.zx81project> [nome] [autor]`
  * `project info <proj.zx81project>`
  * `project list <proj.zx81project>`
  * `project add-source <proj.zx81project> <arquivo_externo> [BASIC|ASM]`
  * `project view-source <proj.zx81project> <nome_fonte>`
  * `project export-source <proj.zx81project> <nome_fonte> <arquivo_saida>`
  * `project add-doc <proj.zx81project> <arquivo_externo>`
  * `project view-doc <proj.zx81project> <titulo>`
* **Visualização na TUI:** A barra de status e o diálogo About passam a indicar dinamicamente o tema ativo configurado no SQLite.
* **Pacote de Distribuição:** Binário compilado e empacotado em `zx81ide-v-0.1.5.zip`.

---

## [v0.1.4] - 2026-10-16
### Título do Lançamento: `zx81ide v0.1.4 [3D Monster Maze] - Alinhamento e ampliacao da borda dos menus TUI`

* **Versão:** `0.1.4`
* **Tag Git:** `v0.1.4`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans, 1981)*
* **Subtítulo da Compilação (Z):** `Alinhamento e ampliacao da borda dos menus TUI`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.4.zip`

---

### 1. Resumo do Lançamento
A versão **v0.1.4** aperfeiçoa o alinhamento das caixas de menus suspensos (dropdowns) no modo TUI. A largura da caixa foi expandida para 22 colunas e foi adotado o cálculo dinâmico de preenchimento (`format_menu_item`), eliminando o problema em que o texto e os atalhos de teclado encostavam ou sobrescreviam a borda vertical direita (`│`).

---

### 2. O Que Foi Entregue Nesta Versão
* **Largura Expandida da Caixa:** Submenus ampliados para 22 colunas (`drop_w = 22`), proporcionando 20 colunas de área interna útil.
* **Formatação Precisa dos Itens:** Inclusão de espaçamento nas extremidades e cálculo exato de caracteres, preservando as bordas intactas.
* **Pacote de Distribuição:** Atualização dos binários e publicação de `zx81ide-v-0.1.4.zip`.

---

## [v0.1.3] - 2026-10-16
### Título do Lançamento: `zx81ide v0.1.3 [3D Monster Maze] - Correcao no parse UTF-8 das bordas no TUI`

* **Versão:** `0.1.3`
* **Tag Git:** `v0.1.3`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans, 1981)*
* **Subtítulo da Compilação (Z):** `Correcao no parse UTF-8 das bordas no TUI`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.3.zip`

---

### 1. Resumo do Lançamento
A versão **v0.1.3** corrige a exibição das bordas dos menus suspensos (cascata) e caixas de diálogo na interface TUI. Foi solucionada a fragmentação indevida de bytes UTF-8 no buffer virtual de tela, garantindo que fontes modernas como **Nerd Fonts (JetBrains Mono)** e emuladores de terminal exibam linhas e cantos contínuos sem caracteres de substituição (losango com ponto de interrogação).

---

### 2. O Que Foi Entregue Nesta Versão
* **Decodificador UTF-8 no ScreenBuffer:** A rotina `put_string` agora agrupa e valida os bytes de cada caractere multi-byte (1 a 4 bytes), alocando exatamente uma célula lógica por coluna na grade do terminal.
* **Bordas Contínuas e Impecáveis:** Desenho consistente de cantos e retas (`┌`, `─`, `┐`, `│`, `└`, `┘`, `╔`, `═`, `╗`, `╚`, `╝`, `║`).
* **Sincronização de Codificação:** Inclusão de `SetConsoleCP(CP_UTF8)` para suporte integral a UTF-8 no Windows.
* **Pacote de Distribuição:** Novo binário empacotado em `zx81ide-v-0.1.3.zip`.

---

## [v0.1.2] - 2026-10-16
### Título do Lançamento: `zx81ide v0.1.2 [3D Monster Maze] - Fundo uniforme com quadriculado no desktop TUI`

* **Versão:** `0.1.2`
* **Tag Git:** `v0.1.2`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans, 1981)*
* **Subtítulo da Compilação (Z):** `Fundo uniforme com quadriculado no desktop TUI`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.2.zip`

---

### 1. Resumo do Lançamento
A versão **v0.1.2** aprimora a experiência visual do modo TUI retro estilo MS-DOS. O padrão quadriculado da área de desktop passa a utilizar uma cor de fundo estritamente uniforme em toda a extensão da tela, removendo a alternância de contraste em blocos xadrez e assegurando uma textura suave, autêntica e agradável ao olhar com caracteres ASCII quadriculados (`░`).

---

### 2. O Que Foi Entregue Nesta Versão
* **Desktop TUI Padronizado:** Aplicação de cor de fundo homogênea com preenchimento texturizado `░` no padrão monocromático Sinclair ZX-81.
* **Consistência Visual:** Eliminação de ruído óptico em consoles de alta resolução.
* **Pacote de Distribuição:** Atualização de binários e geração de `zx81ide-v-0.1.2.zip`.

---

## [v0.1.1] - 2026-10-16
### Título do Lançamento: `zx81ide v0.1.1 [3D Monster Maze] - CLI interativo e modo TUI retro estilo MS-DOS`

* **Versão:** `0.1.1`
* **Tag Git:** `v0.1.1`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans, 1981)*
* **Subtítulo da Compilação (Z):** `CLI interativo e modo TUI retro estilo MS-DOS`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.1.zip`

---

### 1. Resumo do Lançamento
A versão **v0.1.1** implementa o módulo principal CLI com passagem de argumentos via linha de comando, o console interativo com comandos dedicados e a introdução da interface visual de terminal **TUI (Text User Interface)**, modelada diretamente na estética dos softwares da era MS-DOS dos anos 1990 (Turbo Vision / Borland C++), respeitando com rigor histórico a paleta do Sinclair ZX-81 composta unicamente por **Preto**, **Branco** e **Cinza**.

---

### 2. O Que Foi Entregue Nesta Versão

#### 2.1. Parser de Linha de Comando e Modo CLI Principal
* Suporte completo a argumentos de linha de comando:
  * `zx81ide --help` ou `-h`: Exibe a ajuda detalhada e lista de opções.
  * `zx81ide --version` ou `-v`: Exibe metadados completos de versão, hardware e codenames.
  * `zx81ide --tui`: Inicia diretamente na interface retro de terminal TUI.
  * `zx81ide --cli`: Inicia explicitamente no console interativo CLI.
* Execução sem parâmetros: Ao chamar apenas `zx81ide.exe`, o programa ingressa diretamente no **Modo CLI Interativo**.

#### 2.2. Console Interativo CLI (`zx81ide>`)
* Prompt interativo em loop contínuo:
  * `help` ou `?`: Lista todos os comandos disponíveis no console.
  * `tui`: Transiciona dinamicamente para a interface retro TUI.
  * `version` ou `ver`: Exibe a versão formatada do sistema.
  * `clear` ou `cls`: Limpa a tela do console.
  * `exit`, `quit` ou `q`: Encerra o programa com elegância.

#### 2.3. Modo TUI Retro Anos 1990 (Estilo MS-DOS Monocromático)
* **Paleta Estrita ZX-81:** Respeito integral à estética dos anos 1980/1990 em preto (`#000000`), branco (`#FFFFFF`) e cinza (`#808080`).
* **Barra Superior de Menus:** Fundo preto com letras em branco, destacando a seleção em branco com letras pretas:
  * `Arquivo` -> Submenu suspenso com opção `Sair (Alt+X)`.
  * `Help` -> Submenu suspenso com opção `About... (F1)`.
* **Área de Trabalho (Desktop):** Padrão quadriculado em arte ASCII e blocos (`░░▒▒`) em cinza e branco, com sombreamento dos menus e janelas.
* **Barra de Status (Rodapé):** Fundo cinza com letras em preto, atalhos de navegação à esquerda e tag de versão alinhada à direita (`| v0.1.1`).
* **Diálogo Modal About (F1):** Janela centralizada com bordas duplas (`╔═╗`, `║ ║`, `╚═╝`), sombra em relevo e botão interativo `[ OK ]`.
* **Navegação Integral por Teclado:** Setas direcionais (esquerda, direita, acima, abaixo), Enter, Esc, F10, F1 e atalhos rápidos (`Alt+X`, `A`, `H`).

---

### 3. Artefatos Gerados

| Arquivo | Descrição |
| :--- | :--- |
| `dist/zx81ide.exe` | Executável atualizado com CLI interativo e TUI retro integrada. |
| `zx81ide-v-0.1.1.zip` | Pacote zip consolidado para distribuição do release. |

---

## [v0.1.0] - 2026-10-16
### Título do Lançamento: `zx81ide v0.1.0 [3D Monster Maze] - Estrutura inicial do projeto, documentação e CLI bootstrap`

* **Versão:** `0.1.0`
* **Tag Git:** `v0.1.0`
* **Marco de Hardware (X):** `Sinclair ZX81 (Foundation)`
* **Jogo Clássico Homenageado (Y):** `3D Monster Maze` *(Malcolm Evans / J.K. Greye Software, 1981 - Considerado o primeiro jogo 3D para computadores domésticos, um marco histórico de engenharia no ZX-81 com 16KB de RAM).*
* **Subtítulo da Compilação (Z):** `Estrutura inicial do projeto, documentação e CLI bootstrap`
* **Arquivo de Distribuição:** `zx81ide-v-0.1.0.zip`

---

### 1. Resumo do Lançamento
Este é o marco inaugural do projeto **zx81ide**. Estabelece as fundações arquiteturais completas em C++17, o sistema de compilação multiplataforma via CMake e automação em PowerShell, a estrutura modular de diretórios e o conjunto completo de documentações vivas que orientarão o desenvolvimento contínuo da ferramenta.

---

### 2. O Que Foi Entregue Nesta Versão

#### 2.1. Arquitetura de Diretórios C/C++
* Configuração modular separando cabeçalhos públicos (`include/`) e fontes (`src/`):
  * `core/`: Tipos fundamentais e versionamento.
  * `assembler/`: Estrutura reservada para integração com o montador Z80 (SJAsmPlus).
  * `compiler/`: Estrutura para pré-processador BASIC e gerador de snapshots `.p`.
  * `emulator/`: Estrutura para o núcleo de emulação EightyOne / ZEsarUX.
  * `ui_tui/`: Camada de interface de terminal com FTXUI.
  * `ui_gui/`: Camada gráfica com Dear ImGui.
  * `cli/`: Processamento e despacho de opções de linha de comando.
* Diretórios de apoio: `third_party/`, `tests/`, `tools/`, `docs/` e `dist/`.

#### 2.2. Núcleo Executável (CLI Bootstrap)
* Implementação do ponto de entrada `src/main.cpp` e cabeçalho `include/core/version.h`.
* Exibição de banner estilizado em arte ASCII e identificação formal do programa, versão semântica, modelo homenageado, jogo codename e subtítulo de compilação.
* Encerramento limpo com retorno `0`.

#### 2.3. Sistema de Build e Automação (`build.ps1`)
* Script `build.ps1` em PowerShell para orquestrar todas as etapas de desenvolvimento:
  * Detecção inteligente do ambiente (CMake, GCC/g++, Clang).
  * Compilação otimizada com suporte a modo Release.
  * Montagem automática do diretório de entrega `dist/` com o binário `zx81ide.exe` e licenças/documentos essenciais.
  * Empacotamento automático do arquivo compactado `zx81ide-v-0.1.0.zip`.
  * Parâmetros disponíveis: `-Clean`, `-Release`.

#### 2.4. Documentação Integrada
* `README.md`: Apresentação completa, tecnologias (Gemini/Antigravity, PowerShell, GCC/Clang, CMake), créditos a projetos de terceiros e convenções de versionamento.
* `SPEC.md`: Especificação técnica de referência do ecossistema.
* `MANUAL.md`: Manual inicial de usuário e desenvolvedor com tabelas de clones (TK82/83/85, CP200, TS1000).
* `OUTLINE.md`: Memória persistente de conversas com a IA para suporte a trocas de ambiente e novos agentes.
* `CHANGELOG.md`: Registro resumido no padrão Keep a Changelog.
* `RELEASE.md`: Este documento com as notas exaustivas de cada lançamento.

---

### 3. Artefatos Gerados no Release

| Arquivo | Descrição |
| :--- | :--- |
| `dist/zx81ide.exe` | Executável principal compilado nativo (x86_64). |
| `zx81ide-v-0.1.0.zip` | Pacote zip completo contendo binário e documentações para uso imediato. |

---

### 4. Como Executar e Validar

```powershell
# Extrair o arquivo compactado ou navegar até a pasta dist/
cd dist
.\zx81ide.exe
```

Saída esperada:
```text
================================================================
  _____  _     ___  _   _     _ _       
 |_  / \/ /___( _ )/ | (_) __| (_) ___  
  / / \  // _ \/ _ \| | | |/ _` | |/ _ \ 
 /___//_/\___/\___/|_| |_|\__,_|_|\___/ 
  Integrated Development Environment for Sinclair ZX-81 & Clones
================================================================
Program Name   : zx81ide
Version        : 0.1.0
Major Model (X): Sinclair ZX81 (Foundation)
Game Codename(Y): 3D Monster Maze
Build Subtitle(Z): Estrutura inicial do projeto, documentacao e CLI bootstrap
----------------------------------------------------------------
Hello, Sinclair ZX-81 World! [READY]
================================================================
```

---

### 5. Próximos Passos (Planejados para `v0.1.1` e `v0.2.0`)
* Implementação do parser de flags na linha de comando (`-h`, `-v`, `--assemble`, `--basic`).
* Estruturação inicial do conversor BASIC Sinclair para snapshot `.p`.
* Integração dos submódulos externos em `third_party/`.
