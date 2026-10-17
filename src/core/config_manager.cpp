#include "core/config_manager.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cstdlib>

namespace zx81ide {
namespace core {

namespace {

std::string trim_str(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

} // anonymous namespace

ConfigManager& ConfigManager::instance() {
    static ConfigManager inst;
    return inst;
}

ConfigManager::ConfigManager() = default;

ConfigManager::~ConfigManager() {
    close();
}

std::string ConfigManager::current_timestamp_iso() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm_buf, &t);
#else
    localtime_r(&t, &tm_buf);
#endif
    std::ostringstream ss;
    ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string ConfigManager::resolve_default_db_path() {
    return "zx81ide.db";
}

bool ConfigManager::initialize(const std::string& db_path) {
    if (initialized_ && db_.is_open()) {
        return true;
    }

    db_path_ = db_path.empty() ? resolve_default_db_path() : db_path;
    if (!db_.open(db_path_)) {
        std::cerr << "[ConfigManager] Erro ao abrir banco de configuracoes: " << db_.last_error() << std::endl;
        return false;
    }

    const std::string schema = 
        "CREATE TABLE IF NOT EXISTS settings ("
        "  key TEXT PRIMARY KEY,"
        "  value TEXT NOT NULL,"
        "  updated_at TEXT NOT NULL"
        ");";

    if (!db_.execute(schema)) {
        std::cerr << "[ConfigManager] Erro ao criar schema de configuracoes: " << db_.last_error() << std::endl;
        return false;
    }

    initialized_ = true;
    return true;
}

void ConfigManager::close() {
    if (db_.is_open()) {
        db_.close();
    }
    initialized_ = false;
}

std::string ConfigManager::get(const std::string& key, const std::string& default_value) {
    if (!initialized_) {
        initialize();
    }

    Statement stmt(db_, "SELECT value FROM settings WHERE key = ?1;");
    if (stmt.is_valid()) {
        stmt.bind_string(1, key);
        if (stmt.step()) {
            return stmt.get_string(0);
        }
    }
    return default_value;
}

bool ConfigManager::set(const std::string& key, const std::string& value) {
    if (!initialized_) {
        initialize();
    }

    Statement stmt(db_, "INSERT INTO settings (key, value, updated_at) VALUES (?1, ?2, ?3) "
                         "ON CONFLICT(key) DO UPDATE SET value = ?2, updated_at = ?3;");
    if (!stmt.is_valid()) {
        return false;
    }

    std::string timestamp = current_timestamp_iso();
    stmt.bind_string(1, key);
    stmt.bind_string(2, value);
    stmt.bind_string(3, timestamp);
    return stmt.execute();
}

bool ConfigManager::has(const std::string& key) {
    if (!initialized_) {
        initialize();
    }

    Statement stmt(db_, "SELECT 1 FROM settings WHERE key = ?1;");
    if (stmt.is_valid()) {
        stmt.bind_string(1, key);
        return stmt.step();
    }
    return false;
}

bool ConfigManager::remove(const std::string& key) {
    if (!initialized_) {
        initialize();
    }

    Statement stmt(db_, "DELETE FROM settings WHERE key = ?1;");
    if (stmt.is_valid()) {
        stmt.bind_string(1, key);
        return stmt.execute();
    }
    return false;
}

std::map<std::string, std::string> ConfigManager::get_all() {
    std::map<std::string, std::string> result;
    if (!initialized_) {
        initialize();
    }

    Statement stmt(db_, "SELECT key, value FROM settings ORDER BY key ASC;");
    while (stmt.is_valid() && stmt.step()) {
        result[stmt.get_string(0)] = stmt.get_string(1);
    }
    return result;
}

bool ConfigManager::is_valid_theme(const std::string& theme_name) {
    std::string t = theme_name;
    std::transform(t.begin(), t.end(), t.begin(), ::tolower);
    return (t == "normal" || t == "invertido" || t == "claro" || t == "escuro");
}

std::vector<std::string> ConfigManager::get_available_themes() {
    return {"normal", "invertido", "claro", "escuro"};
}

std::string ConfigManager::get_theme() {
    return get("theme", "normal");
}

bool ConfigManager::set_theme(const std::string& theme_name) {
    std::string t = theme_name;
    std::transform(t.begin(), t.end(), t.begin(), ::tolower);
    if (!is_valid_theme(t)) {
        return false;
    }
    return set("theme", t);
}

bool ConfigManager::is_theme_configured() {
    return has("theme");
}

bool ConfigManager::prompt_theme_selection_if_needed() {
    if (!initialized_) {
        initialize();
    }

    if (is_theme_configured()) {
        return true;
    }

    std::cout << "\n================================================================" << std::endl;
    std::cout << "          zx81ide - Configuracao Inicial de Tema                " << std::endl;
    std::cout << "================================================================" << std::endl;
    std::cout << "O banco de configuracoes (" << db_path_ << ") foi criado.\n";
    std::cout << "Por favor, selecione o tema de exibicao desejado:\n\n";
    std::cout << "  [1] normal     - Sinclair ZX-81 Padrao (texto branco/cinza em fundo preto)\n";
    std::cout << "  [2] invertido  - Sinclair Invertido (texto preto em fundo branco)\n";
    std::cout << "  [3] claro      - Tema Claro\n";
    std::cout << "  [4] escuro     - Tema Escuro\n\n";
    std::cout << "Digite sua opcao [1-4] (ou pressione Enter para 'normal'): ";

    std::string line;
    if (!std::getline(std::cin, line)) {
        // Se entrada nao estiver interativa ou for EOF, salva padrao normal
        set_theme("normal");
        std::cout << "normal\n";
        return true;
    }

    std::string input = trim_str(line);
    std::string selected_theme = "normal";

    if (input == "1" || input == "normal") {
        selected_theme = "normal";
    } else if (input == "2" || input == "invertido") {
        selected_theme = "invertido";
    } else if (input == "3" || input == "claro") {
        selected_theme = "claro";
    } else if (input == "4" || input == "escuro") {
        selected_theme = "escuro";
    } else if (input.empty()) {
        selected_theme = "normal";
    } else {
        std::cout << "[AVISO] Opcao '" << input << "' invalida. Aplicando tema padrao 'normal'.\n";
        selected_theme = "normal";
    }

    set_theme(selected_theme);
    std::cout << "[OK] Tema '" << selected_theme << "' salvo com sucesso no banco de dados!\n";
    std::cout << "----------------------------------------------------------------\n\n";
    return true;
}

bool ConfigManager::get_compiler_labels_mode() {
    std::string v = get("compiler.labels_mode", "false");
    std::transform(v.begin(), v.end(), v.begin(), ::tolower);
    return (v == "true" || v == "1" || v == "yes" || v == "sim");
}

bool ConfigManager::set_compiler_labels_mode(bool enabled) {
    return set("compiler.labels_mode", enabled ? "true" : "false");
}

int ConfigManager::get_compiler_start_line() {
    std::string v = get("compiler.start_line", "10");
    try {
        int n = std::stoi(v);
        if (n >= 0 && n <= 9999) return n;
    } catch (...) {}
    return 10;
}

bool ConfigManager::set_compiler_start_line(int line) {
    if (line < 0 || line > 9999) return false;
    return set("compiler.start_line", std::to_string(line));
}

int ConfigManager::get_compiler_line_increment() {
    std::string v = get("compiler.line_increment", "2");
    try {
        int n = std::stoi(v);
        if (n >= 1 && n <= 9990) return n;
    } catch (...) {}
    return 2;
}

bool ConfigManager::set_compiler_line_increment(int inc) {
    if (inc < 1 || inc > 9990) return false;
    return set("compiler.line_increment", std::to_string(inc));
}

} // namespace core
} // namespace zx81ide
