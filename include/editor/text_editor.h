#ifndef ZX81IDE_TEXT_EDITOR_H
#define ZX81IDE_TEXT_EDITOR_H

#include <string>

namespace zx81ide {
namespace editor {

// Apresentacao do Editor:
// - CLI_STANDALONE: Aberto a partir da linha de comando ou console CLI interativo
// - TUI_INTEGRATED: Integrado como janela de desktop dentro da interface TUI
enum class EditorPresentation {
    CLI_STANDALONE,
    TUI_INTEGRATED
};

// Tipo de Editor:
// - FREE_TEXT: Editor livre de textos moderno padrao Edit MS-DOS / Turbo Pascal 6 / Turbo C++
// - CLASSIC_ZX81: Editor no estilo original do Sinclair ZX-81 (linha inferior, tokens, cursor K/L)
enum class EditorStyle {
    FREE_TEXT,
    CLASSIC_ZX81
};

struct EditorOptions {
    std::string file_path;
    EditorPresentation presentation = EditorPresentation::CLI_STANDALONE;
    EditorStyle style = EditorStyle::FREE_TEXT;
    int initial_inner_width = 80;
    int initial_inner_height = 25;
};

// Ponto de entrada principal para o editor de texto
int run_text_editor(const std::string& file_path, 
                    EditorPresentation presentation = EditorPresentation::CLI_STANDALONE,
                    EditorStyle style = EditorStyle::FREE_TEXT);

} // namespace editor
} // namespace zx81ide

#endif // ZX81IDE_TEXT_EDITOR_H
