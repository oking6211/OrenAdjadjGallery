#include "DatabaseAccess.h"
#include "Picture.h"

void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	int albumId = getID("ALBUMS", albumName);
	if (albumId == -1)
	{
		std::cerr << "Album not found!" << std::endl;
		return;
	}
	// get picId
	std::string sql = "SELECT ID FROM PICTURES WHERE NAME = '" + pictureName + "' AND ALBUM_ID = " + std::to_string(albumId) + ";";
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
	int albumId = getID("ALBUMS", albumName);

	// get picId where albumId matches
	std::string sql = "SELECT ID FROM PICTURES WHERE NAME = '" + pictureName + "' AND ALBUM_ID = " + std::to_string(albumId) + ";";
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

	// delete tag where userId and picId match
	std::string deleteSql = "DELETE FROM TAGS WHERE USER_ID = " + std::to_string(userId) +
		" AND PICTURE_ID = " + std::to_string(picId) + ";";

	if (sqlite3_exec(this->galleryDb, deleteSql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error deleting tag: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}

void DatabaseAccess::printUsers()
{
	std::string sql = "SELECT * FROM USERS";
	char* errMsg = nullptr;

	if (sqlite3_exec(this->galleryDb, sql.c_str(), [](void* NotUsed, int argc, char** argv, char** azColName) -> int
		{
			for (int i = 0; i < argc; i++)
			{
				std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << "\t";
			}
			std::cout << std::endl;
			return 0;
		}, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error printing users: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}

User DatabaseAccess::getUser(int userId)
{
	std::string sql = "SELECT ID, NAME FROM USERS WHERE ID = " + std::to_string(userId) + ";";
	User user(-1, "default");
	char* errMsg = nullptr;

	auto callback = [](void* data, int argc, char** argv, char** colNames) -> int
		{
			if (argc == 2 && argv[0] && argv[1]) 
			{
				User* user = static_cast<User*>(data);
				user->setId(std::stoi(argv[0]));
				user->setName(argv[1]);
			}
			return 0;
		};

	if (sqlite3_exec(this->galleryDb, sql.c_str(), callback, &user, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error retrieving user: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	return user;
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

	if (sqlite3_exec(this->galleryDb, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) 
	{
		std::cerr << "Error deleting user: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}

bool DatabaseAccess::doesUserExists(int userId)
{
	std::string sql = "SELECT COUNT(*) FROM Users WHERE ID = " + std::to_string(userId);
	char* errMsg = nullptr;
	int count = 0;

	if (sqlite3_exec(this->galleryDb, sql.c_str(), [](void* data, int argc, char** argv, char**) -> int
		{
			if (argc > 0 && argv[0])
			{
				*static_cast<int*>(data) = std::stoi(argv[0]);
			}
			return 0;
		}, &count, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error checking user existence: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	return count > 0;
}

int DatabaseAccess::countAlbumsOwnedOfUser(const User& user)
{
	std::string sql = "SELECT COUNT(*) FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId()) + ";";
	int count = 0;
	char* errMsg = nullptr;

	if (sqlite3_exec(this->galleryDb, sql.c_str(), [](void* data, int argc, char** argv, char**)->int 
		{
		*static_cast<int*>(data) = std::stoi(argv[0]);
		return 0;
		}, &count, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error counting albums: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	return count;
}

int DatabaseAccess::countAlbumsTaggedOfUser(const User& user)
{
	std::string sql =
		"SELECT COUNT(DISTINCT ALBUMS_ID) FROM ALBUMS "
		"JOIN Pictures ON ALBUMS_ID = PICTURES.ALBUMS_ID "
		"JOIN TAGS ON PICTURE_ID = TAGS.PICTURE_ID "
		"WHERE TAGS.USER_ID = " + std::to_string(user.getId()) + ";";

	int count = 0;
	char* errMsg = nullptr;

	if (sqlite3_exec(this->galleryDb, sql.c_str(), [](void* data, int argc, char** argv, char**)->int 
		{
		*static_cast<int*>(data) = std::stoi(argv[0]);
		return 0;
		}, &count, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error counting tagged albums: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	return count;
}

int DatabaseAccess::countTagsOfUser(const User& user)
{
	std::string sql =
		"SELECT COUNT(*) FROM TAGS "
		"WHERE USER_ID = " + std::to_string(user.getId()) + ";";

	int count = 0;
	char* errMsg = nullptr;

	if (sqlite3_exec(this->galleryDb, sql.c_str(), [](void* data, int argc, char** argv, char**)->int
		{
		*static_cast<int*>(data) = std::stoi(argv[0]);
		return 0;
		}, &count, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error counting tags: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	return count;
}

float DatabaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	std::string sql =
		"SELECT AVG(tag_count) FROM ("
		"SELECT COUNT(TAGS.PICTURE_ID) AS tag_count FROM ALBUMS "
		"JOIN PICTURES ON ALBUMS.ALBUMS_ID = PICTURES.ALBUMS_ID "
		"JOIN TAGS ON PICTURES.PICTURE_ID = TAGS.PICTURE_ID "
		"WHERE TAGS.USER_ID = " + std::to_string(user.getId()) +
		" GROUP BY ALBUMS.ALBUMS_ID);";

	float avg = 0.0f;
	char* errMsg = nullptr;

	auto callback = [](void* data, int argc, char** argv, char** colNames) -> int
		{
		if (argc > 0 && argv[0]) 
		{
			*static_cast<float*>(data) = std::stof(argv[0]);
		}
		return 0;
		};

	if (sqlite3_exec(this->galleryDb, sql.c_str(), callback, &avg, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error retrieving average tags per album: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	return avg;
}

User DatabaseAccess::getTopTaggedUser()
{
	std::string sql =
		"SELECT USER_ID, USER_NAME, COUNT(TAGS.PICTURE_ID) AS tag_count "
		"FROM TAGS "
		"JOIN USERS ON TAGS.USER_ID = USERS.USER_ID "
		"GROUP BY USER_ID "
		"ORDER BY tag_count DESC "
		"LIMIT 1;";

	User topUser(-1, "default");
	char* errMsg = nullptr;

	auto callback = [](void* data, int argc, char** argv, char** colNames) -> int 
		{
		if (argc > 0) 
		{
			User* user = static_cast<User*>(data);
			user->setId(std::stoi(argv[0])); 
			user->setName(argv[1]);          
		}
		return 0;
		};

	if (sqlite3_exec(this->galleryDb, sql.c_str(), callback, &topUser, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error retrieving top tagged user: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	return topUser;
}

Picture DatabaseAccess::getTopTaggedPicture()
{
	std::string sql =
		"SELECT PICTURES.PICTURE_ID, PICTURES.PICTURE_NAME, COUNT(TAGS.PICTURE_ID) AS tag_count "
		"FROM TAGS "
		"JOIN PICTURES ON TAGS.PICTURE_ID = PICTURES.PICTURE_ID "
		"GROUP BY PICTURES.PICTURE_ID "
		"ORDER BY tag_count DESC "
		"LIMIT 1;";

	Picture topPicture(-1, "default");
	char* errMsg = nullptr;

	auto callback = [](void* data, int argc, char** argv, char** colNames) -> int
		{
			if (argc > 0)
			{
				Picture* picture = static_cast<Picture*>(data);
				picture->setId(std::stoi(argv[0]));
				picture->setName(argv[1]);
			}
			return 0;
		};

	if (sqlite3_exec(this->galleryDb, sql.c_str(), callback, &topPicture, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error retrieving top tagged picture: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	return topPicture;
}

std::list<Picture> DatabaseAccess::getTaggedPicturesOfUser(const User& user)
{
	std::string sql =
		"SELECT PICTURES.PICTURE_ID, PICTURES.PICTURE_NAME "
		"FROM TAGS "
		"JOIN PICTURES ON TAGS.PICTURE_ID = PICTURES.PICTURE_ID "
		"WHERE TAGS.USER_ID = " + std::to_string(user.getId()) + ";";

	std::list<Picture> taggedPictures;
	char* errMsg = nullptr;

	auto callback = [](void* data, int argc, char** argv, char** colNames) -> int
		{
			if (argc > 0) 
			{
				std::list<Picture>* pictures = static_cast<std::list<Picture>*>(data);
				pictures->emplace_back(std::stoi(argv[0]), argv[1]); 
			}
			return 0;
		};

	if (sqlite3_exec(this->galleryDb, sql.c_str(), callback, &taggedPictures, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error retrieving tagged pictures for user: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	return taggedPictures;
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
		return 0;
	}
	if (file_exist != 0)
	{
		this->galleryDb = nullptr;
		std::cout << "No table was found!" << std::endl;
		return 0;
	}
	return true;
}

void DatabaseAccess::close()
{
	sqlite3_close(this->galleryDb);
	this->galleryDb = nullptr;
}

void DatabaseAccess::clear()
{
	
	std::string sqlTags = "DELETE FROM TAGS;";
	std::string sqlPictures = "DELETE FROM PICTURES;";
	std::string sqlAlbums = "DELETE FROM ALBUMS;";
	std::string sqlUsers = "DELETE FROM USERS;";

	char* errMsg = nullptr;


	if (sqlite3_exec(this->galleryDb, sqlTags.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error clearing TAGS table: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}


	if (sqlite3_exec(this->galleryDb, sqlPictures.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error clearing PICTURES table: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}


	if (sqlite3_exec(this->galleryDb, sqlAlbums.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error clearing ALBUMS table: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}


	if (sqlite3_exec(this->galleryDb, sqlUsers.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error clearing USERS table: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	
}

int DatabaseAccess::getID(std::string tableName, std::string variableName)
{
	std::string sql = "SELECT id FROM " + tableName + " WHERE name = '" + variableName + "';";
	int id = -1;
	char* errMsg = nullptr;

	
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

const std::list<Album> DatabaseAccess::getAlbums()
{
	std::list<Album> albums;
	std::string sql = "SELECT ID, NAME, USER_ID, CREATION_DATE FROM ALBUMS;";
	char* errMsg = nullptr;

	auto callback = [](void* data, int argc, char** argv, char** azColName) -> int
		{
			std::list<Album>* albums = static_cast<std::list<Album>*>(data);

			if (argc == 4)
			{
				int ownerId = std::stoi(argv[2]);
				std::string name = argv[1];
				std::string creationTime = argv[3];

				Album album(ownerId, name, creationTime);
				albums->push_back(album);
			}
			return 0;
		};

	if (sqlite3_exec(this->galleryDb, sql.c_str(), callback, &albums, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error retrieving albums: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	return albums;
}

const std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user)
{
	std::list<Album> albums;
	std::string sql = "SELECT ID, NAME, USER_ID, CREATION_DATE FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId()) + ";";
	char* errMsg = nullptr;
	
	
	
	
		std::cout << "User Name is:" << user.getName() << std::endl;
	


	auto callback = [](void* data, int argc, char** argv, char** azColName) -> int
		{
			std::list<Album>* albums = static_cast<std::list<Album>*>(data);

			if (argc == 4)
			{
				int ownerId = std::stoi(argv[2]);
				std::string name = argv[1];
				std::string creationTime = argv[3];

				Album album(ownerId, name, creationTime);
				albums->push_back(album);
			}
			return 0;
		};

	if (sqlite3_exec(this->galleryDb, sql.c_str(), callback, &albums, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error retrieving albums: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	return albums;
}

void DatabaseAccess::createAlbum(const Album& album)
{
	std::string sql = "INSERT INTO ALBUMS (NAME, USER_ID, CREATION_DATE) VALUES ('"
		+ album.getName() + "', "
		+ std::to_string(album.getOwnerId()) + ", '"
		+ album.getCreationDate() + "');";
	char* errMsg = nullptr;
	if (sqlite3_exec(this->galleryDb, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error creating album: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}

void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	std::string sql = "DELETE FROM ALBUMS WHERE NAME = '" + albumName + "' AND USER_ID = " + std::to_string(userId) + ";";

	if (sqlite3_exec(this->galleryDb, sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
	{
		std::cerr << "Error executing SQL command! " << std::endl;
		return;
	}

	std::cout << "Album deleted successfully." << std::endl;
}

bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	std::string sql = "SELECT COUNT(*) FROM ALBUMS WHERE NAME = '" + albumName + "' AND USER_ID = " + std::to_string(userId) + ";";
	char* errMsg = nullptr;
	int count = 0;

	auto callback = [](void* data, int argc, char** argv, char** azColName) -> int
		{
			int* count = static_cast<int*>(data);
			if (argc == 1)
			{
				*count = std::stoi(argv[0]);
			}
			return 0;
		};

	if (sqlite3_exec(this->galleryDb, sql.c_str(), callback, &count, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error checking if album exists: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		return false;
	}

	return count > 0;//return true if there is an album
}

Album DatabaseAccess::openAlbum(const std::string& albumName)
{
	// load album details
	std::string sql = "SELECT ID, NAME, USER_ID, CREATION_DATE FROM ALBUMS WHERE NAME = '" + albumName + "';";
	char* errMsg = nullptr;
	Album album;

	auto callback = [](void* data, int argc, char** argv, char** azColName) -> int
		{
			Album* album = static_cast<Album*>(data);

			if (argc == 4)
			{
				int ownerId = std::stoi(argv[2]);
				std::string name = argv[1];
				std::string creationTime = argv[3];
				*album = Album(ownerId, name, creationTime);
			}
			return 0;
		};

	if (sqlite3_exec(this->galleryDb, sql.c_str(), callback, &album, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error retrieving album: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		return album;
	}

	// load pictures for album
	std::string picturesSql = "SELECT ID, NAME, ALBUM_ID, LOCATION, CREATION_DATE FROM PICTURES WHERE ALBUM_ID = (SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "');";

	auto picturesCallback = [](void* data, int argc, char** argv, char** azColName) -> int
		{
			Album* album = static_cast<Album*>(data);

			if (argc == 5)
			{
				int pictureId = std::stoi(argv[0]);
				std::string pictureName = argv[1];
				int albumId = std::stoi(argv[2]);
				std::string location = argv[3];
				std::string creationDate = argv[4];

				
				Picture pic(pictureId, pictureName, location, creationDate);
				if (!album->doesPictureExists(pictureName))
				{
					album->addPicture(pic);
				}
			}
			return 0;
		};

	if (sqlite3_exec(this->galleryDb, picturesSql.c_str(), picturesCallback, &album, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error retrieving pictures: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	// now load the tags 
	std::string tagsSql =
		"SELECT PICTURES.NAME, TAGS.USER_ID "
		"FROM TAGS "
		"JOIN PICTURES ON TAGS.PICTURE_ID = PICTURES.ID "
		"JOIN ALBUMS ON PICTURES.ALBUM_ID = ALBUMS.ID "
		"WHERE ALBUMS.NAME = '" + albumName + "';";

	auto tagsCallback = [](void* data, int argc, char** argv, char** azColName) -> int
		{
			Album* album = static_cast<Album*>(data);

			if (argc == 2) 
			{
				std::string pictureName = argv[0]; 
				int userId = std::stoi(argv[1]);   

				
				Picture pic = album->getPicture(pictureName);

				
				pic.tagUser(userId);

				
				album->removePicture(pictureName);
				album->addPicture(pic);
			}
			return 0;
		};

	if (sqlite3_exec(this->galleryDb, tagsSql.c_str(), tagsCallback, &album, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error retrieving tags: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}

	return album;
}

void DatabaseAccess::closeAlbum(Album& pAlbum)
{

}

void DatabaseAccess::printAlbums()
{
	std::string sql = "SELECT NAME, USER_ID, CREATION_DATE FROM ALBUMS;";
	char* errMsg = nullptr;

	auto callback = [](void* data, int argc, char** argv, char** azColName) -> int
		{
			if (argc == 3)
			{
				std::string name = argv[0];
				int ownerId = std::stoi(argv[1]);
				std::string creationDate = argv[2];

				std::cout << "Album Name: " << name << "\n"
					<< "Owner ID: " << ownerId << "\n"
					<< "Creation Date: " << creationDate << "\n"
					<< "---------------------------\n";
			}
			return 0;
		};

	if (sqlite3_exec(this->galleryDb, sql.c_str(), callback, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error retrieving albums: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}

void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
	int albumId = getID("ALBUMS", albumName);
	if (albumId == -1)
	{
		std::cerr << "Album not found!" << std::endl;
		return;
	}

	std::string sql = "INSERT INTO PICTURES (ALBUM_ID, NAME, LOCATION, CREATION_DATE) VALUES ("
		+ std::to_string(albumId) + ", '" + picture.getName() + "', '" + picture.getPath()
		+ "', strftime('%d/%m/%Y', 'now'));";


	char* errMsg = nullptr;

	if (sqlite3_exec(this->galleryDb, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error inserting picture: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}

void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
	int albumId = getID("ALBUMS", albumName);
	if (albumId == -1)
	{
		std::cerr << "Album not found!" << std::endl;
		return;
	}
	std::string sql = "DELETE FROM PICTURES WHERE ALBUM_ID = " + std::to_string(albumId) + " AND NAME = '" + pictureName + "';";
	char* errMsg = nullptr;

	if (sqlite3_exec(this->galleryDb, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
	{
		std::cerr << "Error deleting picture: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
}
