/////////////////////////////////////////////////////////////////////
// CheckOut.cpp - Implements to browse the repository              //
// ver 1.0                                                         //
// Jing Qi, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////
/*
*
* Required Files:
* ---------------
* CheckOut.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2018
* - first release
*/
#include "CheckOut.h"

using namespace repository;
using namespace NoSqlDb;
//----< constructor, save checkout info >---------------------------------------------
CheckOut::CheckOut(DbCore<Payload> &db, CheckOutInfo &coi) 
	: db_(db)
	, coi_(coi)
	, versionManager_(db, coi_.fileName(), coi_.nameSpace()) {

}
//----< clone one file >---------------------------------------------
bool CheckOut::cloneOneFile(std::string dst) {
	std::vector<int> versions = versionManager_.getExistedVersionNumbers();
	if (std::find(versions.begin(), versions.end(), coi_.version())
		== versions.end()) {
		return false;
	}
	std::string key(coi_.nameSpace() + "::" + coi_.fileName() + "." + std::to_string(coi_.version()));
	FileSystem::File::copy(db_[key].payLoad().dir() + coi_.fileName() + "." + std::to_string(coi_.version()), 
		dst + coi_.fileName());
	return true;
}
//----< clone all files with dependency >-----------------------------------
std::vector<std::string> CheckOut::cloneFilesWithDependency(std::string dst) {
	cloneOneFile(dst);
	DbWalker w(db_);
	bool result = true;
	std::vector<std::string> dependencies = w.dependency(
		coi_.nameSpace() + "::" +
		coi_.fileName() + "." +
		std::to_string(coi_.version()));
	for (auto key : dependencies) {
		CheckOutInfo sub_coi(
			db_[key].payLoad().version(),
			db_[key].name(),
			db_[key].payLoad().nameSpace(),
			db_[key].payLoad().packageName()
		);
		CheckOut sub_co(db_, sub_coi);
		result = sub_co.cloneOneFile(dst);
	}
	return dependencies;
}

#ifdef TEST_CHECKOUT
int main()
{
	// retrive the database
	std::ifstream f;
	std::string xml;
	std::string line;
	f.open("../Repository/repoDataBase.xml");
	while (getline(f, line)) {
		xml.append(line);
	}
	f.close();
	DbCore<Payload> db;
	Persist<Payload> persist(db);
	persist.fromXml(xml);

	showDb(db);
	std::cout << std::endl;

	CheckOutInfo coi(1, "IPayload.h", "NoSqlDb", "Payload");
	CheckOut co(db, coi);
	co.cloneOneFile("../temp/"); // <---

	CheckOutInfo coi1(1, "RepositoryCore.cpp", "repository", "RepositoryCore");
	CheckOut co1(db, coi1);
	co1.cloneFilesWithDependency("../temp/"); // <---
	std::cout << std::endl;
    return 0;
}

#endif