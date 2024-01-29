#include "data_base.h"
#include <assert.h>
int changes = 0;
bool contains(const std::unordered_map<std::string, Result>& map, std::string key) {
    for(std::pair<std::string, Result> pair : map) {
	if(pair.first == key) return true;
    }
    return false;
}
const char* read_word(const char* words, int word) {
    int cursor = 0;
    int word_found = 0;
    while(word_found < word) {
	if(words[cursor] == '\0') {
	    word_found++;
	}
	cursor++;
    } 
    return words + cursor;
}

DataBase::DataBase(const char* file)
{
    uint32_t rc = sqlite3_open(file, &this->db);
    if (rc)
    {
	    std::cout << "Konnte die Datenbank nicht oeffnen\n" << sqlite3_errmsg(this->db) << "\n";
    }
    else
    {
	    std::cout << "Datenbank erfolgreich geoeffnet\n";
    }
}

DataBase::~DataBase()
{
    sqlite3_close(db);
}

Result::Result(std::string entries, std::string col_names, size_t num_cols, size_t num_rows):
    entries(entries), col_names(col_names), num_cols(num_cols), num_rows(num_rows) {
    }

Result::Result() {
    entries = "";
    col_names = "";
    num_cols = 0;
    num_rows = 0;
}

void Result::print(const char* prefix) {
    std::cout << prefix;
    std::cout << "num_rows = " << num_rows << ", num_cols = " << num_cols << "\n";
    for(int i = 0; i < num_cols; ++i) {
	std::cout << read_word(col_names.c_str(), i) << "|";
    }
    for(int i = 0; i < num_rows; ++i) {
	for(int j = 0; j < num_cols; ++j) {
	    std::cout << read_word(entries.c_str(), j * i * num_cols) << "|";
	}
	std::cout << "\n";
    }
}

uint32_t DataBase::sql_exec(const char* sql, int(*callback)(void*, int, char**, char**), void* data)
{
    uint32_t rc;
    char* zErrMsg = 0;
    std::string sql_string;
    if (trans_start) {
	    trans_start = false;
	    sql_string = "Begin; ";
    }
    sql_string += sql;
    sql_string += ";";
    rc = sqlite3_exec(this->db, sql_string.c_str(), callback, data, &zErrMsg);
    if (rc != SQLITE_OK) {
	    std::cout << "SQL Fehler :" << zErrMsg << " \n";
	    sqlite3_free(zErrMsg);
    }
    else {
	    size_t changes = sqlite3_changes(db);
	    if (changes) {
		    std::cout << "changes to the database: " << changes << "\n";
		    save_needed = true;
	    }
    }
    return rc;
}

const Result& DataBase::select(const char* tables, const char* cols, const char* tail)
{
    std::string query = "Select ";
    query += cols;
    query += " from ";
    query += tables;
    query += " ";
    query += tail;
    return sql_query(query.c_str());
}

void DataBase::save_db()
{
    Result r;
    sql_exec("COMMIT;", callback_result, &r);
    std::cout << "commit!\n";
    trans_start = true;
    save_needed = false;
}

void DataBase::rollback_db()
{
    Result r;
    sql_exec("ROLLBACK;", callback_result, &r);
    std::cout << "rollback!\n";
    trans_start = true;
    save_needed = false;
}

const Result& DataBase::sql_query(const char* query, ...)
{
    if (query[0] != 's' && query[0] != 'S') {
	    cache.clear();
	    std::cout << "cache clear!\n";
    }
    Result r;
    static char buf[QUERY_LIMIT] = { 0 };
    va_list args;
    va_start(args, query);
    vsnprintf(buf, QUERY_LIMIT, query, args);
    va_end(args);
    if (!contains(cache, buf)) {
	    std::cout << "========\n";
	    std::cout << "db query = " << buf << "\n";
	    std::cout << "========\n";
	    sql_exec(buf, callback_result, &r);
	    cache[buf] = r;
    }
    return cache[buf];
}

int DataBase::callback_result(void* data, int argc, char** argv, char** azColName)
{
    Result* r = (Result*)data;
    r->num_cols = argc;
    r->num_rows++;
    for (size_t i = 0; i < argc; ++i) {
	    r->col_names += azColName[i];
	    r->col_names += '\0';
	    r->entries += argv[i] ? argv[i] : "";
	    r->entries += '\0';
    }
    return 0;
}

