# RELEASE NOTES - zx81ide

Arquivo oficial de registro e detalhamento de lançamentos do **zx81ide**.

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
