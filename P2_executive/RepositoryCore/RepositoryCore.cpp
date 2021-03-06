/////////////////////////////////////////////////////////////////////
// RepositoryCore.cpp - eazy ways to manage the repo                 //
// ver 1.0                                                         //
// Jing Qi, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////

#include "RepositoryCore.h"
#include <fstream>

using namespace repository;
using namespace NoSqlDb;
#ifdef TEST_REPOSITORYCORE
int main()
{
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
	RepoCore repo(db);
	CheckInInfo cii;
	cii.author("J. Qi");
	cii.discription("CheckOut.cpp version 2");
	cii.filename("CheckOut.cpp");
	cii.nameSpace("repository");
	cii.package("CheckOut");
	cii.children().push_back("repository::CheckOut.h.1");
	CheckIn ci = repo.checkIn(cii);
	cii.discription("modified CheckOut.cpp version 2");
	repo.checkInMetadataModify(ci, cii);
	repo.checkInSubmitFile(ci, "../LocalFiles/CheckOut.cpp");
	repo.checkInClose(ci);
	CheckOutInfo coi;
	coi.fileName("CheckOut.cpp");
	coi.nameSpace("repository");
	coi.packageName("CheckOut");
	coi.version(2);
	CheckOut co = repo.checkOut(coi);
	repo.checkOutCloneOneFile(co, "../temp/");
	repo.checkOntCloneFilesWithDependency(co, "../temp/");
	Browse b = repo.browse();
	//repo.browseFileInApp(b, "repository::CheckOut.h.1");
	repo.browseFileInCMD(b, "repository::CheckOut.h.1");
	Query<Payload> q(db);
	std::string n = "NoSqlDb";
	auto isNameSpace = [&n](DbElement<Payload>& elem) {
		return (elem.payLoad()).nameSpace() == n;
	};
	q.select(isNameSpace);
	repo.browsePackagesByQuery(b, q, 1);

    return 0;
}

#endif