#include "core/project.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace zx81ide {
namespace core {

Project::Project() = default;

Project::Project(const std::string& project_path) {
    open(project_path);
}

Project::~Project() {
    close();
}

Project::Project(Project&& other) noexcept
    : db_(std::move(other.db_)), path_(std::move(other.path_)) {
}

Project& Project::operator=(Project&& other) noexcept {
    if (this != &other) {
        close();
        db_ = std::move(other.db_);
        path_ = std::move(other.path_);
    }
    return *this;
}

std::string Project::current_timestamp_iso() {
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

bool Project::initialize_schema() {
    const std::string schema = 
        "CREATE TABLE IF NOT EXISTS project_meta ("
        "  key TEXT PRIMARY KEY,"
        "  value TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS project_settings ("
        "  key TEXT PRIMARY KEY,"
        "  value TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS sources ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL UNIQUE,"
        "  type TEXT NOT NULL,"
        "  content TEXT NOT NULL,"
        "  created_at TEXT NOT NULL,"
        "  updated_at TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS screens ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL UNIQUE,"
        "  format TEXT NOT NULL,"
        "  data BLOB,"
        "  notes TEXT,"
        "  created_at TEXT NOT NULL,"
        "  updated_at TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS binaries ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL UNIQUE,"
        "  target_type TEXT NOT NULL,"
        "  data BLOB,"
        "  size_bytes INTEGER NOT NULL,"
        "  build_timestamp TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS manuals ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  title TEXT NOT NULL UNIQUE,"
        "  format TEXT NOT NULL,"
        "  content TEXT NOT NULL,"
        "  created_at TEXT NOT NULL,"
        "  updated_at TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS assets ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL UNIQUE,"
        "  asset_type TEXT NOT NULL,"
        "  data BLOB,"
        "  created_at TEXT NOT NULL,"
        "  updated_at TEXT NOT NULL"
        ");";

    return db_.execute(schema);
}

bool Project::create_new(const std::string& path, 
                         const std::string& name, 
                         const std::string& author, 
                         const std::string& description) {
    close();
    path_ = path;

    if (!db_.open(path_)) {
        return false;
    }

    if (!initialize_schema()) {
        return false;
    }

    std::string now = current_timestamp_iso();

    ProjectMeta meta;
    meta.name = name;
    meta.version = "1.0.0";
    meta.author = author;
    meta.description = description;
    meta.target_model = "Sinclair ZX81 (16K RAM)";
    meta.default_source = "main.bas";
    meta.created_at = now;
    meta.updated_at = now;

    if (!update_metadata(meta)) {
        return false;
    }

    // Criar arquivo fonte inicial padrao ZX-81 BASIC
    std::string default_basic = 
        "10 REM *** ZX-81 BASIC PROJECT ***\n"
        "20 PRINT \"HELLO FROM " + name + "!\"\n"
        "30 PRINT \"DEVELOPED WITH ZX81IDE\"\n"
        "40 FOR I = 1 TO 10\n"
        "50 PRINT \"COUNT: \"; I\n"
        "60 NEXT I\n"
        "70 STOP\n";

    add_source("main.bas", "BASIC", default_basic);

    // Criar manual inicial padrao
    std::string default_manual = 
        "# " + name + "\n\n"
        "Projeto criado no zx81ide.\n\n"
        "## Autor\n" + (author.empty() ? "Nao informado" : author) + "\n\n"
        "## Descricao\n" + (description.empty() ? "Projeto ZX-81" : description) + "\n\n"
        "## Instrucoes de Execucao\n"
        "- Compilar com zx81ide para gerar imagem .p\n"
        "- Carregar no emulador EightyOne ou hardware real ZX-81/TK85\n";

    add_manual("MANUAL.md", "MARKDOWN", default_manual);

    return true;
}

bool Project::open(const std::string& path) {
    close();
    path_ = path;

    if (!db_.open(path_)) {
        return false;
    }

    if (!initialize_schema()) {
        return false;
    }

    return true;
}

void Project::close() {
    if (db_.is_open()) {
        db_.close();
    }
    path_.clear();
}

bool Project::is_open() const {
    return db_.is_open();
}

ProjectMeta Project::get_metadata() {
    ProjectMeta meta;
    if (!db_.is_open()) return meta;

    Statement stmt(db_, "SELECT key, value FROM project_meta;");
    while (stmt.is_valid() && stmt.step()) {
        std::string k = stmt.get_string(0);
        std::string v = stmt.get_string(1);

        if (k == "name") meta.name = v;
        else if (k == "version") meta.version = v;
        else if (k == "author") meta.author = v;
        else if (k == "description") meta.description = v;
        else if (k == "target_model") meta.target_model = v;
        else if (k == "default_source") meta.default_source = v;
        else if (k == "created_at") meta.created_at = v;
        else if (k == "updated_at") meta.updated_at = v;
    }
    return meta;
}

bool Project::update_metadata(const ProjectMeta& meta) {
    if (!db_.is_open()) return false;

    db_.begin_transaction();

    auto set_meta_val = [this](const std::string& k, const std::string& v) {
        Statement stmt(db_, "INSERT INTO project_meta (key, value) VALUES (?1, ?2) "
                             "ON CONFLICT(key) DO UPDATE SET value = ?2;");
        if (stmt.is_valid()) {
            stmt.bind_string(1, k);
            stmt.bind_string(2, v);
            stmt.execute();
        }
    };

    set_meta_val("name", meta.name);
    set_meta_val("version", meta.version);
    set_meta_val("author", meta.author);
    set_meta_val("description", meta.description);
    set_meta_val("target_model", meta.target_model);
    set_meta_val("default_source", meta.default_source);
    set_meta_val("created_at", meta.created_at);
    set_meta_val("updated_at", current_timestamp_iso());

    return db_.commit();
}

bool Project::set_setting(const std::string& key, const std::string& value) {
    if (!db_.is_open()) return false;

    Statement stmt(db_, "INSERT INTO project_settings (key, value) VALUES (?1, ?2) "
                         "ON CONFLICT(key) DO UPDATE SET value = ?2;");
    if (!stmt.is_valid()) return false;

    stmt.bind_string(1, key);
    stmt.bind_string(2, value);
    return stmt.execute();
}

std::string Project::get_setting(const std::string& key, const std::string& default_val) {
    if (!db_.is_open()) return default_val;

    Statement stmt(db_, "SELECT value FROM project_settings WHERE key = ?1;");
    if (stmt.is_valid()) {
        stmt.bind_string(1, key);
        if (stmt.step()) {
            return stmt.get_string(0);
        }
    }
    return default_val;
}

// ----------------------------------------------------------------------------
// Fontes
// ----------------------------------------------------------------------------

bool Project::add_source(const std::string& name, const std::string& type, const std::string& content) {
    if (!db_.is_open()) return false;

    std::string now = current_timestamp_iso();
    Statement stmt(db_, "INSERT INTO sources (name, type, content, created_at, updated_at) "
                         "VALUES (?1, ?2, ?3, ?4, ?5) "
                         "ON CONFLICT(name) DO UPDATE SET type = ?2, content = ?3, updated_at = ?5;");
    if (!stmt.is_valid()) return false;

    stmt.bind_string(1, name);
    stmt.bind_string(2, type);
    stmt.bind_string(3, content);
    stmt.bind_string(4, now);
    stmt.bind_string(5, now);
    return stmt.execute();
}

bool Project::update_source(const std::string& name, const std::string& content) {
    if (!db_.is_open()) return false;

    std::string now = current_timestamp_iso();
    Statement stmt(db_, "UPDATE sources SET content = ?1, updated_at = ?2 WHERE name = ?3;");
    if (!stmt.is_valid()) return false;

    stmt.bind_string(1, content);
    stmt.bind_string(2, now);
    stmt.bind_string(3, name);
    return stmt.execute();
}

bool Project::get_source(const std::string& name, SourceFile& out_source) {
    if (!db_.is_open()) return false;

    Statement stmt(db_, "SELECT id, name, type, content, created_at, updated_at FROM sources WHERE name = ?1;");
    if (!stmt.is_valid()) return false;

    stmt.bind_string(1, name);
    if (stmt.step()) {
        out_source.id = stmt.get_int64(0);
        out_source.name = stmt.get_string(1);
        out_source.type = stmt.get_string(2);
        out_source.content = stmt.get_string(3);
        out_source.created_at = stmt.get_string(4);
        out_source.updated_at = stmt.get_string(5);
        return true;
    }
    return false;
}

std::vector<SourceFile> Project::list_sources() {
    std::vector<SourceFile> list;
    if (!db_.is_open()) return list;

    Statement stmt(db_, "SELECT id, name, type, content, created_at, updated_at FROM sources ORDER BY name ASC;");
    while (stmt.is_valid() && stmt.step()) {
        SourceFile sf;
        sf.id = stmt.get_int64(0);
        sf.name = stmt.get_string(1);
        sf.type = stmt.get_string(2);
        sf.content = stmt.get_string(3);
        sf.created_at = stmt.get_string(4);
        sf.updated_at = stmt.get_string(5);
        list.push_back(sf);
    }
    return list;
}

bool Project::delete_source(const std::string& name) {
    if (!db_.is_open()) return false;

    Statement stmt(db_, "DELETE FROM sources WHERE name = ?1;");
    if (!stmt.is_valid()) return false;

    stmt.bind_string(1, name);
    return stmt.execute();
}

// ----------------------------------------------------------------------------
// Telas
// ----------------------------------------------------------------------------

bool Project::add_screen(const std::string& name, const std::string& format, 
                         const std::vector<uint8_t>& data, const std::string& notes) {
    if (!db_.is_open()) return false;

    std::string now = current_timestamp_iso();
    Statement stmt(db_, "INSERT INTO screens (name, format, data, notes, created_at, updated_at) "
                         "VALUES (?1, ?2, ?3, ?4, ?5, ?6) "
                         "ON CONFLICT(name) DO UPDATE SET format = ?2, data = ?3, notes = ?4, updated_at = ?6;");
    if (!stmt.is_valid()) return false;

    stmt.bind_string(1, name);
    stmt.bind_string(2, format);
    stmt.bind_blob(3, data);
    stmt.bind_string(4, notes);
    stmt.bind_string(5, now);
    stmt.bind_string(6, now);
    return stmt.execute();
}

std::vector<ScreenFile> Project::list_screens() {
    std::vector<ScreenFile> list;
    if (!db_.is_open()) return list;

    Statement stmt(db_, "SELECT id, name, format, notes, updated_at FROM screens ORDER BY name ASC;");
    while (stmt.is_valid() && stmt.step()) {
        ScreenFile scr;
        scr.id = stmt.get_int64(0);
        scr.name = stmt.get_string(1);
        scr.format = stmt.get_string(2);
        scr.notes = stmt.get_string(3);
        scr.updated_at = stmt.get_string(4);
        list.push_back(scr);
    }
    return list;
}

// ----------------------------------------------------------------------------
// Binarios
// ----------------------------------------------------------------------------

bool Project::add_binary(const std::string& name, const std::string& target_type, 
                         const std::vector<uint8_t>& data) {
    if (!db_.is_open()) return false;

    std::string now = current_timestamp_iso();
    Statement stmt(db_, "INSERT INTO binaries (name, target_type, data, size_bytes, build_timestamp) "
                         "VALUES (?1, ?2, ?3, ?4, ?5) "
                         "ON CONFLICT(name) DO UPDATE SET target_type = ?2, data = ?3, size_bytes = ?4, build_timestamp = ?5;");
    if (!stmt.is_valid()) return false;

    stmt.bind_string(1, name);
    stmt.bind_string(2, target_type);
    stmt.bind_blob(3, data);
    stmt.bind_int64(4, static_cast<int64_t>(data.size()));
    stmt.bind_string(5, now);
    return stmt.execute();
}

std::vector<BinaryFile> Project::list_binaries() {
    std::vector<BinaryFile> list;
    if (!db_.is_open()) return list;

    Statement stmt(db_, "SELECT id, name, target_type, size_bytes, build_timestamp FROM binaries ORDER BY name ASC;");
    while (stmt.is_valid() && stmt.step()) {
        BinaryFile bf;
        bf.id = stmt.get_int64(0);
        bf.name = stmt.get_string(1);
        bf.target_type = stmt.get_string(2);
        bf.size_bytes = stmt.get_int64(3);
        bf.build_timestamp = stmt.get_string(4);
        list.push_back(bf);
    }
    return list;
}

// ----------------------------------------------------------------------------
// Manuais
// ----------------------------------------------------------------------------

bool Project::add_manual(const std::string& title, const std::string& format, const std::string& content) {
    if (!db_.is_open()) return false;

    std::string now = current_timestamp_iso();
    Statement stmt(db_, "INSERT INTO manuals (title, format, content, created_at, updated_at) "
                         "VALUES (?1, ?2, ?3, ?4, ?5) "
                         "ON CONFLICT(title) DO UPDATE SET format = ?2, content = ?3, updated_at = ?5;");
    if (!stmt.is_valid()) return false;

    stmt.bind_string(1, title);
    stmt.bind_string(2, format);
    stmt.bind_string(3, content);
    stmt.bind_string(4, now);
    stmt.bind_string(5, now);
    return stmt.execute();
}

std::vector<ManualFile> Project::list_manuals() {
    std::vector<ManualFile> list;
    if (!db_.is_open()) return list;

    Statement stmt(db_, "SELECT id, title, format, content, updated_at FROM manuals ORDER BY title ASC;");
    while (stmt.is_valid() && stmt.step()) {
        ManualFile mf;
        mf.id = stmt.get_int64(0);
        mf.title = stmt.get_string(1);
        mf.format = stmt.get_string(2);
        mf.content = stmt.get_string(3);
        mf.updated_at = stmt.get_string(4);
        list.push_back(mf);
    }
    return list;
}

// ----------------------------------------------------------------------------
// Resumo do Projeto
// ----------------------------------------------------------------------------

ProjectSummary Project::get_summary() {
    ProjectSummary summary;
    if (!db_.is_open()) return summary;

    summary.meta = get_metadata();

    std::string s_count = db_.execute_scalar("SELECT COUNT(*) FROM sources;", "0");
    std::string scr_count = db_.execute_scalar("SELECT COUNT(*) FROM screens;", "0");
    std::string bin_count = db_.execute_scalar("SELECT COUNT(*) FROM binaries;", "0");
    std::string man_count = db_.execute_scalar("SELECT COUNT(*) FROM manuals;", "0");

    summary.source_count = std::stoull(s_count);
    summary.screen_count = std::stoull(scr_count);
    summary.binary_count = std::stoull(bin_count);
    summary.manual_count = std::stoull(man_count);

    return summary;
}

} // namespace core
} // namespace zx81ide
