# RELEASE NOTES - zx81ide

Arquivo oficial de registro e detalhamento de lançamentos do **zx81ide**.

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
