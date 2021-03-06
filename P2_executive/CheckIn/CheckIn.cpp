/////////////////////////////////////////////////////////////////////
// CheckIn.cpp - Implements to Check-in the repository             //
// ver 1.0                                                         //
// Jing Qi, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////
/*
*
* Required Files:
* ---------------
* CheckIn.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2018
* - first release
*/
#include "CheckIn.h"

using namespace NoSqlDb;
using namespace repository;

//----< constructor, check the item and add the item into the db >---------------------------
CheckIn::CheckIn(DbCore<Payload> &db, CheckInInfo& cii)
	: db_(db)
	, cii_(cii)
	, versionManager_(db, cii.filename(), cii.nameSpace()) {
	// check if children exists and closed
	for (auto item : cii_.children()) {
		if ((!db_.contains(item)) || (db[item].payLoad().status() == true))
			return;
	}

	// create metadata
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

//----< close the checkin >----------------------------------
void CheckIn::close() {
	pl_.status(false);
	open_status_ = false;
	elem_.dateTime(DateTime().now());
	elem_.payLoad(pl_);
	db_[opened_key_] = elem_;
}

//----< change the data >---------------------------------------------
void CheckIn::changeMetaData(CheckInInfo& cii) {
	if (!open_status_) return;

	// check if children exists
	for (auto item : cii.children()) {
		if (!db_.contains(item))
			return;
	}
	cii_ = cii;
	pl_.author(cii_.author());
	pl_.children(cii_.children());
	elem_.descrip(cii_.discription());
	elem_.dateTime(DateTime().now());
	elem_.payLoad(pl_);
	db_[opened_key_] = elem_;
}
//----< submit a file >---------------------------------------------
void CheckIn::submitFile(std::string dirAndFile) {
	std::string vm = std::to_string(versionManager_.currentVersion());
	std::string dst = dir_ + cii_.filename() + "." + vm;
	FileSystem::File::copy(dirAndFile, dst);
}

#ifdef TEST_CHECKIN
//----< easy way to create an item >-----------------------
void wrapFile(DbCore<Payload>& db,
	std::string a,
	std::string d,
	std::string f,
	std::string n,
	std::string p,
	std::vector<std::string> c,
	std::string sf) 
{
	CheckInInfo cii(a, d, f, n, p);
	cii.children(c);
	CheckIn ci(db, cii);
	ci.submitFile(sf);
	ci.close();
}

//----< construct the database >---------------------------------------------
void constructDatabase(DbCore<Payload>& db) {
	std::vector<std::string> children;
	wrapFile(db, "J. Fawcett", "Payload", "IPayload.h", "NoSqlDb", "Payload", children, "../LocalFiles/IPayload.h");
	children.push_back("NoSqlDb::IPayload.h.1");
	wrapFile(db, "J. Qi", "Payload", "Payload.h", "NoSqlDb", "Payload", children, "../LocalFiles/Payload.h");
	children.push_back("NoSqlDb::Payload.h.1");
	wrapFile(db, "J. Qi", "Payload", "Payload.cpp", "NoSqlDb", "Payload", children, "../LocalFiles/Payload.cpp");
	children.clear();
	children.push_back("NoSqlDb::Payload.h.1");
	wrapFile(db, "J. Fawcett", "dnc", "DbCore.h", "NoSqlDb", "DbCore", children, "../LocalFiles/DbCore.h");
	children.clear();
	children.push_back("NoSqlDb::DbCore.h.1");
	wrapFile(db, "J. Fawcett", "dnc", "DbCore.cpp", "NoSqlDb", "DbCore", children, "../LocalFiles/DbCore.cpp");
	children.clear();
	wrapFile(db, "J. Fawcett", "fs", "FileSystem.h", "FileSystem", "FileSystem", children, "../LocalFiles/FileSystem.h");
	children.push_back("FileSystem::FileSystem.h.1");
	wrapFile(db, "J. Fawcett", "fs", "FileSystem.CPP", "FileSystem", "FileSystem", children, "../LocalFiles/FileSystem.h");
	children.clear();
	children.push_back("NoSqlDb::Payload.h.1");
	children.push_back("NoSqlDb::DbCore.h.1");
	children.push_back("FileSystem::FileSystem.h.1");
	wrapFile(db, "J. Qi", "ci", "CheckIn.h", "repository", "CheckIn", children, "../LocalFiles/CheckIn.h");
	children.clear();
	children.push_back("repository::CheckIn.h.1");
	wrapFile(db, "J. Qi", "ci", "CheckIn.cpp", "repository", "CheckIn", children, "../LocalFiles/CheckIn.cpp");
	children.clear();
	children.push_back("NoSqlDb::Payload.h.1");
	children.push_back("NoSqlDb::DbCore.h.1");
	children.push_back("FileSystem::FileSystem.h.1");
	wrapFile(db, "J. Qi", "co", "CheckOut.h", "repository", "CheckOut", children, "../LocalFiles/CheckOut.h");
	children.clear();
	children.push_back("repository::CheckOut.h.1");
	wrapFile(db, "J. Qi", "co", "CheckOut.cpp", "repository", "CheckOut", children, "../LocalFiles/CheckOut.cpp");
	children.clear();
	children.push_back("NoSqlDb::Payload.h.1");
	children.push_back("NoSqlDb::DbCore.h.1");
	children.push_back("repository::CheckIn.h.1");
	children.push_back("repository::CheckOut.h.1");
	wrapFile(db, "J. Qi", "rc", "RepositoryCore.h", "repository", "RepositoryCore", children, "../LocalFiles/RepositoryCore.h");
	children.clear();
	children.push_back("repository::RepositoryCore.h.1");
	wrapFile(db, "J. Qi", "co", "RepositoryCore.cpp", "repository", "RepositoryCore", children, "../LocalFiles/RepositoryCore.cpp");
}

int main()
{
	// create the database
	DbCore<Payload> db;
	constructDatabase(db);
	std::vector<std::string> children;
	children.push_back("NoSqlDb::Payload.h.1");
	children.push_back("NoSqlDb::DbCore.h.1");
	children.push_back("FileSystem::FileSystem.h.1");
	CheckInInfo cii("J. Qi", "CheckOut.h version 2", "CheckOut.h", "repository", "CheckOut");
	cii.children(children);
	CheckIn ci(db, cii);
	ci.submitFile("../LocalFiles/CheckOut.h");

	//check in
	CheckInInfo cii2("J. Qi", "CheckIn.h version 2", "CheckIn.h", "repository", "CheckIn");
	cii2.children(children);
	CheckIn ci2(db, cii2);
	ci2.submitFile("../LocalFiles/CheckIn.h");

	showDb(db);
	//save it into xml
	Persist<Payload> persist(db);
	std::ofstream f;
	f.open("../Repository/repoDataBase.xml");
	f << persist.toXml();
	f.close();
	std::cout << std::endl << FileSystem::Directory::getCurrentDirectory() << std::endl;
    
	return 0;
}

#endif