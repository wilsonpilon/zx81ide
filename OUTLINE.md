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
