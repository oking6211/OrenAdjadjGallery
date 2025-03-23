#include "DatabaseAccess.h"

void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	int albumId = getID("ALBUM", albumName);

	// get picId
	std::string sql = "SELECT ID FROM PICTURE WHERE name = '" + pictureName + "' AND ALBUM_ID = " + std::to_string(albumId) + ";";
	int picId = -1;
	char* errMsg = nullptr;

	auto callback = [](void* data, int argc, char** argv, char** azColName) -> int 
		{
		if (argc > 0 && argv[0]) 
		{
			*(int*)data = std::stoi(argv[0]);
		}
			return 0;
		};

	if (sqlite3_exec(this->galleryDb, sql.c_str(), callback, &picId, &errMsg) != SQLITE_OK) 
	{
		std::cerr << "Error executing SQL: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		return;
	}

	if (picId == -1) 
	{
		std::cerr << "Picture not found in the specified album.\n";
		return;
	}

	//now that i have the picture id, can tag user
	std::string insertSql = "INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (" +
		std::to_string(picId) + ", " + std::to_string(userId) + ");";

	if (sqlite3_exec(this->galleryDb, insertSql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error inserting tag: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}


void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	int albumId = getID("ALBUM", albumName);

	// get picId where albumId matches
	std::string sql = "SELECT id FROM PICTURE WHERE name = '" + pictureName + "' AND albumId = " + std::to_string(albumId) + ";";
	int picId = -1;
	char* errMsg = nullptr;

	auto callback = [](void* data, int argc, char** argv, char** azColName) -> int 
		{
		if (argc > 0 && argv[0])
		{
			*(int*)data = std::stoi(argv[0]);
		}
		return 0;
		};

	if (sqlite3_exec(this->galleryDb, sql.c_str(), callback, &picId, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error executing SQL: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		return;
	}

	if (picId == -1
		) {
		std::cerr << "Picture not found in the specified album.\n";
		return;
	}

	// delete tag where userId and picId match
	std::string deleteSql = "DELETE FROM TAGS WHERE userId = " + std::to_string(userId) +
		" AND pictureId = " + std::to_string(picId) + ";";

	if (sqlite3_exec(this->galleryDb, deleteSql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error deleting tag: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}

void DatabaseAccess::printUsers()
{
	
}

User DatabaseAccess::getUser(int userId)
{

}

void DatabaseAccess::createUser(User& user)
{
	std::string sql = "INSERT INTO USERS (NAME) VALUES ('" + user.getName() + "');";
	char* errMsg = nullptr;

	if (sqlite3_exec(this->galleryDb, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error inserting user: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}

void DatabaseAccess::deleteUser(const User& user)
{
	std::string sql = "DELETE FROM USERS WHERE NAME = '" + user.getName() + "';";
	char* errMsg = nullptr;

	if (sqlite3_exec(this->galleryDb, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::cerr << "Error deleting user: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}

bool DatabaseAccess::open()	
{
	std::string dbFileName = "galleryDB.sqlite";
	int file_exist = _access(dbFileName.c_str(), 0);
	int res = sqlite3_open(dbFileName.c_str(), &this->galleryDb);
	if (res != SQLITE_OK)
	{
		this->galleryDb = nullptr;
		std::cout << "Failed to open DB" << std::endl;
		return -1;
	}
	if (file_exist != 0)
	{
		this->galleryDb = nullptr;
		std::cout << "No table was found!" << std::endl;
		return -1;
	}
}

void DatabaseAccess::close()
{
	sqlite3_close(this->galleryDb);
	this->galleryDb = nullptr;
}


int DatabaseAccess::getID(std::string tableName, std::string variableName)
{
	std::string sql = "SELECT id FROM " + tableName + " WHERE name = '" + variableName + "';";
	int id = -1;
	char* errMsg = nullptr;

	// callback function to store the result in id
	auto callback = [](void* data, int argc, char** argv, char** azColName) -> int
		{
		if (argc > 0 && argv[0]) 
		{
			*(int*)data = std::stoi(argv[0]);  
		}
		return 0;
		};

	
	if (sqlite3_exec(this->galleryDb, sql.c_str(), callback, &id, &errMsg) != SQLITE_OK) 
	{
		std::cerr << "Error executing SQL: " << errMsg << std::endl;
		sqlite3_free(errMsg); 
	}

	return id;
}


void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	std::string sql = "DELETE FROM ALBUMS WHERE name = '" + albumName + "' AND user_id = " + std::to_string(userId) + ";";

	// execute the command
	if (sqlite3_exec(this->galleryDb, sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) 
	{
		std::cerr << "Error executing SQL command! " << std::endl;
		return;
	}

	std::cout << "Album deleted successfully." << std::endl;
}
