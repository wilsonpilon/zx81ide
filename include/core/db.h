#ifndef ZX81IDE_CORE_DB_H
#define ZX81IDE_CORE_DB_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <functional>

struct sqlite3;
struct sqlite3_stmt;

namespace zx81ide {
namespace core {

class Statement;

class Database {
public:
    Database();
    explicit Database(const std::string& path);
    ~Database();

    // Desabilitar copia, permitir movimentacao
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&& other) noexcept;
    Database& operator=(Database&& other) noexcept;

    bool open(const std::string& path);
    void close();
    bool is_open() const;

    bool execute(const std::string& sql);
    std::string execute_scalar(const std::string& sql, const std::string& default_val = "");
    
    bool begin_transaction();
    bool commit();
    bool rollback();

    int64_t last_insert_rowid() const;
    std::string last_error() const;
    const std::string& get_path() const { return path_; }
    sqlite3* handle() const { return db_; }

private:
    sqlite3* db_ = nullptr;
    std::string path_;
    mutable std::string last_error_msg_;
};

class Statement {
public:
    Statement(Database& db, const std::string& sql);
    ~Statement();

    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;

    bool is_valid() const { return stmt_ != nullptr; }

    bool bind_string(int index, const std::string& value);
    bool bind_int(int index, int value);
    bool bind_int64(int index, int64_t value);
    bool bind_blob(int index, const std::vector<uint8_t>& blob);
    bool bind_null(int index);

    bool step(); // Retorna true se houver uma linha (SQLITE_ROW)
    bool execute(); // Retorna true se a execucao foi concluida com sucesso (SQLITE_DONE ou SQLITE_ROW)
    bool reset();

    std::string get_string(int col);
    int get_int(int col);
    int64_t get_int64(int col);
    std::vector<uint8_t> get_blob(int col);
    bool is_null(int col);

    int column_count() const;
    std::string column_name(int col) const;

private:
    sqlite3_stmt* stmt_ = nullptr;
    Database& db_;
};

} // namespace core
} // namespace zx81ide

#endif // ZX81IDE_CORE_DB_H
