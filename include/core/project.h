#ifndef ZX81IDE_CORE_PROJECT_H
#define ZX81IDE_CORE_PROJECT_H

#include "core/db.h"
#include <string>
#include <vector>
#include <cstdint>

namespace zx81ide {
namespace core {

struct ProjectMeta {
    std::string name;
    std::string version = "1.0.0";
    std::string author;
    std::string description;
    std::string target_model = "Sinclair ZX81 (16K RAM)";
    std::string default_source = "main.bas";
    std::string created_at;
    std::string updated_at;
};

struct SourceFile {
    int64_t id = 0;
    std::string name;
    std::string type; // BASIC, ASM, HEADER, INCLUDE
    std::string content;
    std::string created_at;
    std::string updated_at;
};

struct ScreenFile {
    int64_t id = 0;
    std::string name;
    std::string format; // ASCII, DFILE_32x24, UDG, HRG
    std::vector<uint8_t> data;
    std::string notes;
    std::string updated_at;
};

struct BinaryFile {
    int64_t id = 0;
    std::string name;
    std::string target_type; // P, BIN, HEX, TZX
    std::vector<uint8_t> data;
    int64_t size_bytes = 0;
    std::string build_timestamp;
};

struct ManualFile {
    int64_t id = 0;
    std::string title;
    std::string format; // MARKDOWN, TEXT
    std::string content;
    std::string updated_at;
};

struct ProjectSummary {
    ProjectMeta meta;
    size_t source_count = 0;
    size_t screen_count = 0;
    size_t binary_count = 0;
    size_t manual_count = 0;
};

class Project {
public:
    Project();
    explicit Project(const std::string& project_path);
    ~Project();

    // Desabilitar copia, permitir movimentacao
    Project(const Project&) = delete;
    Project& operator=(const Project&) = delete;
    Project(Project&& other) noexcept;
    Project& operator=(Project&& other) noexcept;

    bool create_new(const std::string& path, 
                    const std::string& name, 
                    const std::string& author = "", 
                    const std::string& description = "");

    bool open(const std::string& path);
    void close();
    bool is_open() const;
    const std::string& get_path() const { return path_; }

    ProjectMeta get_metadata();
    bool update_metadata(const ProjectMeta& meta);

    bool set_setting(const std::string& key, const std::string& value);
    std::string get_setting(const std::string& key, const std::string& default_val = "");

    // Gestao de Fontes (BASIC e ASM)
    bool add_source(const std::string& name, const std::string& type, const std::string& content);
    bool update_source(const std::string& name, const std::string& content);
    bool get_source(const std::string& name, SourceFile& out_source);
    std::vector<SourceFile> list_sources();
    bool delete_source(const std::string& name);

    // Gestao de Telas (Screens)
    bool add_screen(const std::string& name, const std::string& format, 
                    const std::vector<uint8_t>& data, const std::string& notes = "");
    std::vector<ScreenFile> list_screens();

    // Gestao de Binarios (Compilados .p, .bin)
    bool add_binary(const std::string& name, const std::string& target_type, 
                    const std::vector<uint8_t>& data);
    std::vector<BinaryFile> list_binaries();

    // Gestao de Manuais e Documentos
    bool add_manual(const std::string& title, const std::string& format, const std::string& content);
    std::vector<ManualFile> list_manuals();

    ProjectSummary get_summary();

    std::string last_error() const { return db_.last_error(); }

private:
    Database db_;
    std::string path_;

    bool initialize_schema();
    std::string current_timestamp_iso();
};

} // namespace core
} // namespace zx81ide

#endif // ZX81IDE_CORE_PROJECT_H
