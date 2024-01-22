#pragma once
#include "sqlite3.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <inttypes.h>

#define INDEX(x,y, width) x + y * width
#define CHAR_LIMIT 1024 * 5
#define QUERY_LIMIT 512

struct Result
{
	std::string entries;
	std::string col_names;
};

class DataBase
{
public:
	
	DataBase(const char* file);
	~DataBase();
	const Result& sql_query(const char* query, ...);
	const Result& select(const char* tables, const char* cols, const char* tail = "");
	void save_db();
	void rollback_db();
	bool save_needed = false;
private:
	sqlite3* db;
	std::unordered_map<std::string, Result> cache;
	// the callback gets called for every entry yogtu get from the database
	static int callback_result(void* data, int argc, char** argv, char** azColName);
	uint32_t sql_exec(const char* sql, int(*callback)(void*, int, char**, char**), void* data);
	bool trans_start = true;
};
