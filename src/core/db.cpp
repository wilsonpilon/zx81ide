#include "core/db.h"
#include "sqlite3.h"
#include <iostream>

namespace zx81ide {
namespace core {

// ============================================================================
// Database
// ============================================================================

Database::Database() = default;

Database::Database(const std::string& path) {
    open(path);
}

Database::~Database() {
    close();
}

Database::Database(Database&& other) noexcept
    : db_(other.db_), path_(std::move(other.path_)), last_error_msg_(std::move(other.last_error_msg_)) {
    other.db_ = nullptr;
}

Database& Database::operator=(Database&& other) noexcept {
    if (this != &other) {
        close();
        db_ = other.db_;
        path_ = std::move(other.path_);
        last_error_msg_ = std::move(other.last_error_msg_);
        other.db_ = nullptr;
    }
    return *this;
}

bool Database::open(const std::string& path) {
    close();
    path_ = path;
    int rc = sqlite3_open(path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        if (db_) {
            last_error_msg_ = sqlite3_errmsg(db_);
            sqlite3_close(db_);
            db_ = nullptr;
        } else {
            last_error_msg_ = "Nao foi possivel alocar memoria para o SQLite.";
        }
        return false;
    }
    // Habilitar foreign keys e WAL mode para performance
    execute("PRAGMA foreign_keys = ON;");
    execute("PRAGMA journal_mode = WAL;");
    return true;
}

void Database::close() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool Database::is_open() const {
    return db_ != nullptr;
}

bool Database::execute(const std::string& sql) {
    if (!db_) {
        last_error_msg_ = "Banco de dados nao esta aberto.";
        return false;
    }

    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        if (err_msg) {
            last_error_msg_ = err_msg;
            sqlite3_free(err_msg);
        } else {
            last_error_msg_ = sqlite3_errmsg(db_);
        }
        return false;
    }
    return true;
}

std::string Database::execute_scalar(const std::string& sql, const std::string& default_val) {
    if (!db_) return default_val;

    Statement stmt(*this, sql);
    if (stmt.is_valid() && stmt.step()) {
        return stmt.get_string(0);
    }
    return default_val;
}

bool Database::begin_transaction() {
    return execute("BEGIN TRANSACTION;");
}

bool Database::commit() {
    return execute("COMMIT;");
}

bool Database::rollback() {
    return execute("ROLLBACK;");
}

int64_t Database::last_insert_rowid() const {
    return db_ ? sqlite3_last_insert_rowid(db_) : 0;
}

std::string Database::last_error() const {
    if (!last_error_msg_.empty()) {
        return last_error_msg_;
    }
    return db_ ? sqlite3_errmsg(db_) : "Nenhum erro registrado.";
}

// ============================================================================
// Statement
// ============================================================================

Statement::Statement(Database& db, const std::string& sql)
    : db_(db) {
    if (!db.handle()) return;

    int rc = sqlite3_prepare_v2(db.handle(), sql.c_str(), -1, &stmt_, nullptr);
    if (rc != SQLITE_OK) {
        stmt_ = nullptr;
    }
}

Statement::~Statement() {
    if (stmt_) {
        sqlite3_finalize(stmt_);
        stmt_ = nullptr;
    }
}

bool Statement::bind_string(int index, const std::string& value) {
    if (!stmt_) return false;
    return sqlite3_bind_text(stmt_, index, value.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK;
}

bool Statement::bind_int(int index, int value) {
    if (!stmt_) return false;
    return sqlite3_bind_int(stmt_, index, value) == SQLITE_OK;
}

bool Statement::bind_int64(int index, int64_t value) {
    if (!stmt_) return false;
    return sqlite3_bind_int64(stmt_, index, value) == SQLITE_OK;
}

bool Statement::bind_blob(int index, const std::vector<uint8_t>& blob) {
    if (!stmt_) return false;
    return sqlite3_bind_blob(stmt_, index, blob.data(), static_cast<int>(blob.size()), SQLITE_TRANSIENT) == SQLITE_OK;
}

bool Statement::bind_null(int index) {
    if (!stmt_) return false;
    return sqlite3_bind_null(stmt_, index) == SQLITE_OK;
}

bool Statement::step() {
    if (!stmt_) return false;
    int rc = sqlite3_step(stmt_);
    return rc == SQLITE_ROW;
}

bool Statement::execute() {
    if (!stmt_) return false;
    int rc = sqlite3_step(stmt_);
    return rc == SQLITE_DONE || rc == SQLITE_ROW;
}

bool Statement::reset() {
    if (!stmt_) return false;
    return sqlite3_reset(stmt_) == SQLITE_OK;
}

std::string Statement::get_string(int col) {
    if (!stmt_) return "";
    const unsigned char* text = sqlite3_column_text(stmt_, col);
    return text ? reinterpret_cast<const char*>(text) : "";
}

int Statement::get_int(int col) {
    if (!stmt_) return 0;
    return sqlite3_column_int(stmt_, col);
}

int64_t Statement::get_int64(int col) {
    if (!stmt_) return 0;
    return sqlite3_column_int64(stmt_, col);
}

std::vector<uint8_t> Statement::get_blob(int col) {
    std::vector<uint8_t> result;
    if (!stmt_) return result;

    const void* ptr = sqlite3_column_blob(stmt_, col);
    int bytes = sqlite3_column_bytes(stmt_, col);
    if (ptr && bytes > 0) {
        const uint8_t* byte_ptr = static_cast<const uint8_t*>(ptr);
        result.assign(byte_ptr, byte_ptr + bytes);
    }
    return result;
}

bool Statement::is_null(int col) {
    if (!stmt_) return true;
    return sqlite3_column_type(stmt_, col) == SQLITE_NULL;
}

int Statement::column_count() const {
    return stmt_ ? sqlite3_column_count(stmt_) : 0;
}

std::string Statement::column_name(int col) const {
    if (!stmt_) return "";
    const char* name = sqlite3_column_name(stmt_, col);
    return name ? name : "";
}

} // namespace core
} // namespace zx81ide
