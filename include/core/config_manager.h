#ifndef ZX81IDE_CORE_CONFIG_MANAGER_H
#define ZX81IDE_CORE_CONFIG_MANAGER_H

#include "core/db.h"
#include <string>
#include <map>
#include <vector>

namespace zx81ide {
namespace core {

class ConfigManager {
public:
    static ConfigManager& instance();

    bool initialize(const std::string& db_path = "");
    void close();

    std::string get(const std::string& key, const std::string& default_value = "");
    bool set(const std::string& key, const std::string& value);
    bool has(const std::string& key);
    bool remove(const std::string& key);
    std::map<std::string, std::string> get_all();

    // Gestao de Temas
    std::string get_theme();
    bool set_theme(const std::string& theme_name);
    bool is_theme_configured();
    bool prompt_theme_selection_if_needed();

    // Configuracoes do Compilador ZXText2P
    bool get_compiler_labels_mode();
    bool set_compiler_labels_mode(bool enabled);
    int get_compiler_start_line();
    bool set_compiler_start_line(int line);
    int get_compiler_line_increment();
    bool set_compiler_line_increment(int inc);

    static bool is_valid_theme(const std::string& theme_name);
    static std::vector<std::string> get_available_themes();

    const std::string& get_database_path() const { return db_path_; }

private:
    ConfigManager();
    ~ConfigManager();
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    Database db_;
    std::string db_path_;
    bool initialized_ = false;

    std::string resolve_default_db_path();
    std::string current_timestamp_iso();
};

} // namespace core
} // namespace zx81ide

#endif // ZX81IDE_CORE_CONFIG_MANAGER_H
