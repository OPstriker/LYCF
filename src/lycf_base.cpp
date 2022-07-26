#include "lycf_base.h"
#include "v0iden.h"
#include <iostream>

void lycf_base::CreateDB(std::string path)
{
	sqlite3* DB;

	int exit = 0;

	exit = sqlite3_open(path.c_str(), &DB);

	sqlite3_close(DB);
}

void lycf_base::CreateTable(std::string path)
{
	sqlite3* DB;

	char* messageError;

	std::string sql =
		"CREATE TABLE IF NOT EXISTS LOCAL("
		"D_ProfilePath		TEXT NOT NULL);";

	try
	{
		int exit = 0;

		sqlite3_open(path.c_str(), &DB);

		sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

		if (exit != SQLITE_OK)
		{
			std::cout << "Failed to Create Table\n";
			sqlite3_free(messageError);
		}
		else
			std::cout << "Successfully Created table\n";
		sqlite3_close(DB);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
	}
}

void lycf_base::InsertData(std::string path)
{
	sqlite3* DB;
	char* messageError;

	std::string sql(
		"INSERT INTO LOCAL (D_ProfilePath) VALUES('OpenVPN Profile Path');"
	);

	int exit = sqlite3_open(path.c_str(), &DB);

	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

	if (exit != SQLITE_OK)
	{
		std::cout << "Failed to Insert Data\n";
		sqlite3_free(messageError);
	}
	else
		std::cout << "Successfully Inserted Data\n";
}

void lycf_base::UpdateData(std::string path)
{
	sqlite3* DB;
	char* messageError;

	std::string sql_update = "UPDATE LOCAL SET D_ProfilePath = '" + v0iden::v0iden::prf_path + "'";

	std::string sql(
		sql_update.c_str()
	);

	int exit = sqlite3_open(path.c_str(), &DB);

	exit = sqlite3_exec(DB, sql.c_str(), 0, 0, &messageError);

	if (exit != SQLITE_OK)
	{
		std::cout << "Unable to Edit Database Data...\n";
		sqlite3_free(messageError);
	}
	else {
		std::cout << "SuccessFully Updated Database...\n";
	};
}

void lycf_base::DeleteData(std::string path)
{
	sqlite3* DB;
	char* messageError;

	std::string sql(
		"DELETE FROM LOCAL;"
	);

	int exit = sqlite3_open(path.c_str(), &DB);

	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

	if (exit != SQLITE_OK)
	{
		std::cout << "Failed to Clear Databse...\n";
		sqlite3_free(messageError);
	}
	else {
		std::cout << "Successfully Cleared Database...\n";
	}
}

void lycf_base::FetchData(std::string path)
{
	sqlite3* DB;
	char* messageError;

	std::string sql(
		"SELECT D_ProfilePath FROM LOCAL;"
	);

	int exit = sqlite3_open(path.c_str(), &DB);

	exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);

	if (exit != SQLITE_OK)
	{
		std::cout << "Failed to Select Data...\n";
		sqlite3_free(messageError);
	}
	else
		std::cout << "Successfully Selected Data...\n";
}

int lycf_base::callback(void* NotUsed, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(azColName[i], "D_ProfilePath") == 0)
		{
			v0iden::v0iden::prf_path = argv[i];
			std::cout << "Your Profile Path: " + v0iden::v0iden::prf_path;
			break;
		}
	}

	std::cout << std::endl;

    return 0;
}
