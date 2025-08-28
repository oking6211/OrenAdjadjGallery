#ifndef DATA_ACCESS_TEST_H
#define DATA_ACCESS_TEST_H

#include <iostream>
#include "sqlite3.h"
#include <string>

class DataAccessTest
{
public:
	DataAccessTest(const std::string& dbPath);
	~DataAccessTest();

	void createTables();
	void insertSampleData();
	void fixTypo();
	void deleteUserData(int userId);

private:
	sqlite3* db;
	bool executeSQL(const std::string& sql);
};

#endif // DATA_ACCESS_TEST_H
