// CheckIn.cpp : Defines the entry point for the console application.
//
#include "CheckIn.h"

using namespace NoSqlDb;
using namespace repository;

CheckIn::CheckIn(DbCore<Payload> &db, CheckInInfo& cii)
	: db_(db)
	, cii_(cii)
	, versionManager_(db, cii.filename(), cii.nameSpace()) {
	opened_key_ = cii_.nameSpace() + "::" 
		+ cii_.filename()
		+ "." + std::to_string(versionManager_.currentVersion());
	dir_ = repoDir_ + cii_.nameSpace() + "/" + cii_.package() + "/" + cii_.filename() + "/";
	
	// Create the repo's directory
	if (!FileSystem::Directory::exists(repoDir_))
		FileSystem::Directory::create(repoDir_);
	FileSystem::Directory::setCurrentDirectory(repoDir_);
	if (!FileSystem::Directory::exists(cii_.nameSpace() + "/"))
		FileSystem::Directory::create(cii_.nameSpace() + "/");
	FileSystem::Directory::setCurrentDirectory("./"+ cii_.nameSpace() + "/");
	if (!FileSystem::Directory::exists(cii_.package() + "/"))
		FileSystem::Directory::create(cii_.package() + "/");
	FileSystem::Directory::setCurrentDirectory("./" + cii_.package() + "/");
	if (!FileSystem::Directory::exists(cii_.filename() + "/"))
		FileSystem::Directory::create(cii_.filename() + "/");
	FileSystem::Directory::setCurrentDirectory("../../");
	
	// Add item into database.
	pl_.dir(dir_); 
	pl_.version(versionManager_.currentVersion());
	pl_.author(cii_.author()); 
	pl_.nameSpace(cii_.nameSpace());
	pl_.children(cii_.children());
	pl_.packageName(cii_.package());
	pl_.status(true);
	elem_.name(cii_.filename());
	elem_.descrip(cii_.discription());
	elem_.payLoad(pl_);
	db_[opened_key_] = elem_;
}

void CheckIn::close() {
	pl_.status(false);
	open_status_ = false;
	elem_.dateTime(DateTime().now());
	elem_.payLoad(pl_);
	db_[opened_key_] = elem_;
}

void CheckIn::changeMetaData(CheckInInfo& cii) {
	if (!open_status_) return;
	cii_ = cii;
	pl_.author(cii_.author());
	pl_.children(cii_.children());
	elem_.descrip(cii_.discription());
	elem_.dateTime(DateTime().now());
	elem_.payLoad(pl_);
	db_[opened_key_] = elem_;
}

void CheckIn::submitFile(std::string dir) {
	FileSystem::File::copy(dir, dir_+cii_.filename()
		+"."+std::to_string(versionManager_.currentVersion()));
}

int main()
{
	DbCore<Payload> db;
	CheckInInfo cii;
	cii.author("jq");
	cii.children().push_back("1");
	cii.children().push_back("2");
	cii.filename("Browse.cpp");
	cii.discription("browsing");
	cii.nameSpace("repository");
	cii.package("Browse");
	CheckIn ci1(db, cii);
	ci1.submitFile("../LocalFiles/Browse.cpp");
	ci1.close();

	CheckIn ci4(db, cii);
	ci4.submitFile("../LocalFiles/Browse.cpp");

	cii.filename("Browse.h");
	CheckIn ci2(db, cii);
	ci2.submitFile("../LocalFiles/Browse.h");

	cii.nameSpace("NoSqlDb");
	cii.filename("IPayload.h");
	cii.package("Payload");
	CheckIn ci3(db, cii);
	Sleep(1000);
	cii.discription("modified");
	ci3.changeMetaData(cii);
	ci3.submitFile("../LocalFiles/IPayload.h");

	showDb(db);
	std::cout << std::endl << FileSystem::Directory::getCurrentDirectory() << std::endl;
    
	return 0;
}

