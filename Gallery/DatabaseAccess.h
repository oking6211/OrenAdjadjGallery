#pragma once
#include "IDataAccess.h"
#include <corecrt_io.h>
#include "sqlite3.h"
class DatabaseAccess : public IDataAccess
{
public:
	// album related
	virtual const std::list<Album> getAlbums();
	virtual const std::list<Album> getAlbumsOfUser(const User& user);
	virtual void createAlbum(const Album& album);
	virtual void deleteAlbum(const std::string& albumName, int userId);
	virtual bool doesAlbumExists(const std::string& albumName, int userId);
	virtual Album openAlbum(const std::string& albumName);
	virtual void closeAlbum(Album& pAlbum);
	virtual void printAlbums();

	// picture related
	virtual void addPictureToAlbumByName(const std::string& albumName, const Picture& picture);
	virtual void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName);
	void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	virtual void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId);

	// user related
	virtual void printUsers();
	virtual User getUser(int userId);
	virtual void createUser(User& user);
	virtual void deleteUser(const User& user);
	virtual bool doesUserExists(int userId);


	// user statistics
	virtual int countAlbumsOwnedOfUser(const User& user);
	virtual int countAlbumsTaggedOfUser(const User& user);
	virtual int countTagsOfUser(const User& user);
	virtual float averageTagsPerAlbumOfUser(const User& user);

	// queries
	virtual User getTopTaggedUser();
	virtual Picture getTopTaggedPicture();
	virtual std::list<Picture> getTaggedPicturesOfUser(const User& user);

	bool open() override;
	void close() override;
	void clear() override;

	//my functions 
	int getID(std::string tableName, std::string varibleName);

private:
	sqlite3* galleryDb;

};