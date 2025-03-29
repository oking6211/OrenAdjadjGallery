#include "DataAccessTest.h"

DataAccessTest::DataAccessTest(const std::string& dbPath)
{
	if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK)
	{
		std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
		db = nullptr;
	}
}

DataAccessTest::~DataAccessTest()
{
	if (db)
		sqlite3_close(db);
}

bool DataAccessTest::executeSQL(const std::string& sql)
{
	char* errMsg = nullptr;
	if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "SQL error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		return false;
	}
	return true;
}

void DataAccessTest::createTables()
{
	std::string sql =
		"CREATE TABLE IF NOT EXISTS USERS ("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
		"NAME TEXT NOT NULL);"

		"CREATE TABLE IF NOT EXISTS ALBUMS ("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
		"NAME TEXT NOT NULL, "
		"USER_ID INTEGER NOT NULL, "
		"CREATION_DATE TEXT NOT NULL, "
		"FOREIGN KEY(USER_ID) REFERENCES USERS(ID));"

		"CREATE TABLE IF NOT EXISTS PICTURES ("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
		"NAME TEXT NOT NULL, "
		"ALBUM_ID INTEGER NOT NULL, "
		"CREATION_DATE TEXT NOT NULL, "
		"LOCATION TEXT NOT NULL, "
		"FOREIGN KEY(ALBUM_ID) REFERENCES ALBUMS(ID));"

		"CREATE TABLE IF NOT EXISTS TAGS ("
		"PICTURE_ID INTEGER NOT NULL, "
		"USER_ID INTEGER NOT NULL, "
		"FOREIGN KEY(PICTURE_ID) REFERENCES PICTURES(ID), "
		"FOREIGN KEY(USER_ID) REFERENCES USERS(ID));";

	executeSQL(sql);
}

void DataAccessTest::insertSampleData()
{
	std::string sql =
		"INSERT INTO USERS (NAME) VALUES ('Alice');"
		"INSERT INTO USERS (NAME) VALUES ('Bob');"
		"INSERT INTO USERS (NAME) VALUES ('Charlie');"

		"INSERT INTO ALBUMS (NAME, USER_ID, CREATION_DATE) VALUES ('Alice_Album', 1, '28/03/2025');"
		"INSERT INTO ALBUMS (NAME, USER_ID, CREATION_DATE) VALUES ('Bob_Album', 2, '28/03/2025');"
		"INSERT INTO ALBUMS (NAME, USER_ID, CREATION_DATE) VALUES ('Charlie_Album', 3, '28/03/2025');"

		"INSERT INTO PICTURES (NAME, ALBUM_ID, CREATION_DATE, LOCATION) VALUES ('Beach', 1, '28/03/2025', 'Images/beach.jpg');"
		"INSERT INTO PICTURES (NAME, ALBUM_ID, CREATION_DATE, LOCATION) VALUES ('Mountain', 1, '28/03/2025', 'Images/mountain.jpg');"

		"INSERT INTO PICTURES (NAME, ALBUM_ID, CREATION_DATE, LOCATION) VALUES ('City', 2, '28/03/2025', 'Images/city.jpg');"
		"INSERT INTO PICTURES (NAME, ALBUM_ID, CREATION_DATE, LOCATION) VALUES ('Park', 2, '28/03/2025', 'Images/park.jpg');"

		"INSERT INTO PICTURES (NAME, ALBUM_ID, CREATION_DATE, LOCATION) VALUES ('Desert', 3, '28/03/2025', 'Images/desert.jpg');"
		"INSERT INTO PICTURES (NAME, ALBUM_ID, CREATION_DATE, LOCATION) VALUES ('Forest', 3, '28/03/2025', 'Images/forest.jpg');"

		"INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (1, 2);"
		"INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (1, 3);"

		"INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (2, 1);"
		"INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (2, 3);"

		"INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (3, 1);"
		"INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (3, 2);"

		"INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (4, 2);"
		"INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (4, 3);"

		"INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (5, 1);"
		"INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (5, 3);"

		"INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (6, 1);"
		"INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (6, 2);";

	executeSQL(sql);
}

void DataAccessTest::fixTypo()
{
	std::string insertTypo = "INSERT INTO PICTURES (NAME, ALBUM_ID) VALUES ('Femily My', 1);";
	executeSQL(insertTypo);

	std::string fixTypo = "UPDATE PICTURES SET NAME = 'Family My' WHERE NAME = 'Femily My';";
	executeSQL(fixTypo);
}

void DataAccessTest::deleteUserData(int userId)
{
	std::string sql =
		"DELETE FROM TAGS WHERE PICTURE_ID IN (SELECT ID FROM PICTURES WHERE ALBUM_ID IN (SELECT ID FROM ALBUMS WHERE USER_ID = " + std::to_string(userId) + "));"
		"DELETE FROM PICTURES WHERE ALBUM_ID IN (SELECT ID FROM ALBUMS WHERE USER_ID = " + std::to_string(userId) + ");"
		"DELETE FROM ALBUMS WHERE USER_ID = " + std::to_string(userId) + ";"
		"DELETE FROM USERS WHERE ID = " + std::to_string(userId) + ";";

	executeSQL(sql);
}
