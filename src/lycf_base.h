#pragma once
#include <string>
#include <sqlite3.h>

class lycf_base
{
public:
	static void CreateDB(std::string path);
	static void CreateTable(std::string path);
	static void InsertData(std::string path);
	static void UpdateData(std::string path);
	static void DeleteData(std::string path);
	static void FetchData(std::string path);
	static int callback(void* NotUsed, int argc, char** argv, char** azColName);

};

