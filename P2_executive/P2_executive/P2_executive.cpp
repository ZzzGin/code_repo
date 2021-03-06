/////////////////////////////////////////////////////////////////////
// P2_executive.cpp                                                  //
// ver 1.0                                                         //
// Jing Qi, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////
/*
*
* Required Files:
* ---------------
* P2_executive.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2018
* - first release
*/
#include "P2_executive.h"
#include <fstream>

using namespace repository;
using namespace NoSqlDb;

//----< R1 >-----------------------------------------------------------
/*
*  - Demonstrate that solution uses C++11
*/
bool TestDb::testR1()
{
	Utilities::title("Demonstrating Requirement #1");
	std::cout << "\n  " << typeid(std::function<bool()>).name()
		<< ", declared in this function, "
		<< "\n  is only valid for C++11 and later versions.";
	return true; // would not compile unless C++11
}
//----< R2 >-----------------------------------------------------------
/*
*  - Cite use of streams and operators new and delete
*/
bool TestDb::testR2()
{
	Utilities::title("Demonstrating Requirement #2");
	std::cout << "\n  A visual examination of all the submitted code "
		<< "will show only\n  use of streams and operators new and delete.";
	return true;
}
//----< R3 >-----------------------------------------------------------
/*
*  - Provide C++ packages
*/
bool TestDb::testR3()
{
	Utilities::title("Demonstrating Requirement #3");
	std::cout << "\n a. TestExecutive can be found here:";
	TestDb::identify();
	std::cout << "\n This package executes this test.\n";
	std::cout << "\n b. RepositoryCore can be found here:";
	RepoCore::identify();
	std::cout << "\n This package provides means to check-in, version,\n"
		<< " browse, and check-out source code packages, which\n"
		<< " would be demostrated in R4f\n";
	std::cout << "\n c. Check-in can be found here:";
	CheckIn::identify();
	std::cout << "\n This package provides functions to Check-in, which\n"
		<< " would be demostrated in R4\n";
	std::cout << "\n d. Check-out can be found here:";
	CheckOut::identify();
	std::cout << "\n This package provides functions to Check-out, which\n"
		<< " would be demostrated in R5b\n";
	std::cout << "\n e. Version can be found here:";
	Version::identify();
	std::cout << "\n Here, let me load the prepared repository. All the items have closed version1.\n"
		<< "CheckIn.h and CheckOut.h have opened version 2.\n";
	std::ifstream f;
	std::string xml;
	std::string line;
	f.open("../Repository/repoDataBase.xml");
	while (getline(f, line)) {
		xml.append(line);
	}
	f.close();
	Persist<Payload> persist(db_);
	persist.fromXml(xml);
	showDb(db_);
	Payload::showDb(db_);
	Version v(db_, "CheckIn.h", "repository");
	std::cout << std::endl << "\n  CheckIn.h's current (opened or next opened) version is:" << v.currentVersion();
	std::cout << std::endl << "  CheckIn.h's Opened Version is:" << v.openedVersion();
	Version v2(db_, "Payload.h", "repository");
	std::cout << std::endl << "\n  Payload.h's current (opened or next opened) version is:" << v2.currentVersion();
	std::cout << std::endl << "  Payload.h's Opened Version is:" << v2.openedVersion() << "\n";
	std::cout << "\n f. Browse can be found here:";
	std::cout << "\n This package provides functions to Browse, which\n"
		<< " would be demostrated in R5\n";
	Browse::identify();
	return true;
}

//----< R4a >-----------------------------------------------------------
/*
*  - append Number to the file
*  - default check-in is open
*/
bool TestDb::testR4a(){
	Utilities::title("Demonstrating Requirement #4a");
	std::cout << "\n  This test R4a is designed to demonstrate the version number appending\n";
	Payload::showDb(db_);
	std::cout << "\n  We can see that Payload.h's status is Open(1). Now we close it.";
	CheckInInfo cii("J. Qi", "CheckIn.h version 2", "CheckIn.h", "repository", "CheckIn");
	std::vector<std::string> children;
	children.push_back("NoSqlDb::Payload.h.1");
	children.push_back("NoSqlDb::DbCore.h.1");
	children.push_back("FileSystem::FileSystem.h.1");
	cii.children(children);
	CheckIn ci(db_, cii);
	ci.close();
	FileSystem::File::remove("../Repository/repository/CheckIn/CheckIn.h/CheckIn.h.3");
	std::cout << "\n  Now the CheckIn.h.2's status is: " << db_["repository::CheckIn.h.2"].payLoad().status();
	std::cout << "\n  Now the files in '../Repository/repository/CheckIn/CheckIn.h/' are:";
	std::vector<std::string> files = FileSystem::Directory::getFiles("../Repository/repository/CheckIn/CheckIn.h/");
	for (auto file : files) {
		std::cout << "\n   - " << file;
	}
	std::cout << "\n  Now we check in a new CheckIn.h";
	CheckIn new_ci(db_, cii);
	new_ci.submitFile("../LocalFiles/CheckIn.h");
	files.clear();
	std::cout << "\n  After checking a new one, the files in '../Repository/repository/CheckIn/CheckIn.h/' are:";
	files = FileSystem::Directory::getFiles("../Repository/repository/CheckIn/CheckIn.h/");
	for (auto file : files) {
		std::cout << "\n   - " << file;
	}
	std::cout << "\n  We can see that a new file is generated appending the version# at the end of the filename.";
	std::cout << "\n\n  At the same time, the status of this check-in, which is the default check-in's status is: "
		<< db_["repository::CheckIn.h.3"].payLoad().status();
	std::cout << "\n  (1 is open, 0 is closed)";
	
	return true;
}

//----< R4b >-----------------------------------------------------------
/*
*  - modify data
*  - close a checkin
*  - closeing child policy
*/
bool TestDb::testR4b() {
	Utilities::title("Demonstrating Requirement #4b");
	std::cout << "\n  Now, repository::CheckIn.h.3's status is: "<< db_["repository::CheckIn.h.3"].payLoad().status();
	std::cout << "\n  The description of this checkin is: "
		<< db_["repository::CheckIn.h.3"].descrip();
	std::cout << "\n  It is wrong because the version is 3. We are going to change it.";
	CheckInInfo cii("J. Qi", "CheckIn.h version 3", "CheckIn.h", "repository", "CheckIn");
	std::vector<std::string> children;
	children.push_back("NoSqlDb::Payload.h.1");
	children.push_back("NoSqlDb::DbCore.h.1");
	children.push_back("FileSystem::FileSystem.h.1");
	cii.children(children); 
	CheckIn ci(db_, cii);
	std::cout << "\n  After modification, the description of this checkin is: "
		<< db_["repository::CheckIn.h.3"].descrip();
	std::cout << "\n  Now we close it";
	ci.close();
	std::cout << "\n  After closing, the status of this checkin is: "
		<< db_["repository::CheckIn.h.3"].payLoad().status();
	std::cout << "\n  Now we trying to modificate the file";
	cii.discription("CheckIn.h version 4");
	CheckIn new_ci(db_, cii);
	new_ci.submitFile("../LocalFiles/CheckIn.h");
	Payload::showDb(db_);
	std::cout << "\n  We can see that version 3 is closed and a new version 4 is created.";
	std::cout << "\n\n  In the Requirement, 'a check-in can only be closed if its dependent"
		<< "packages are all present in the Repository, with closed check-ins.'";
	std::cout << "\n  This would cause problems: What if A depends on B, B depends on A and"
		<< "\n  both of them are opened? None of them can be closed in this closing policy.";
	std::cout << "\n  In my design, the dependency checking focuses on CheckIn. We can not check in an"
		<< "\n  item whose dependent item is not closed.";
	int sizeBefore = db_.size();
	std::cout << "\n  Now we check in a item dependents on repository::CheckIn.h.4, which is opened now.";
	children.push_back("repository::CheckIn.h.4");
	cii.children(children);
	cii.filename("thisCantBeAdd");
	CheckIn another_new_ci(db_, cii);
	int sizeAfter = db_.size();
	std::cout << "\n  the db size before the check-in is: " + std::to_string(sizeBefore);
	std::cout << "\n  the db size after the check-in is:  " + std::to_string(sizeAfter);
	std::cout << "\n  This check-in is not successful.";
	return true;
}
//----< R5a >-----------------------------------------------------------
/*
*  - browsing packages description
*  - browsing a query
*  - display files
*/
bool TestDb::testR5a() {
	Utilities::title("Demonstrating Requirement #5a");
	std::cout << "\n  -Browse all the items.";
	Browse b(db_);
	Query<Payload> q(db_);
	auto all = [](DbElement<Payload>& elem) {
		return true;
	};
	q.select(all);
	b.browsePackagesByQuery(q, 0);

	std::cout << "\n\n  -Browse items from a query and show all text";
	Query<Payload> q1(db_);
	std::string n = "IPayload.h";
	int v = 1;
	auto isNameSpace = [&n, &v](DbElement<Payload>& elem) {
		return (elem.name() == n) && (elem.payLoad().version() == v);
	};
	q1.select(isNameSpace);
	b.browsePackagesByQuery(q1, 1);
	return true;
}
//----< R5b >-----------------------------------------------------------
/*
*  - RepoCore
*  - Check Out
*/
bool TestDb::testR5b() {
	Utilities::title("Demonstrating Requirement #5b");
	std::cout << "\n  This test shows:\n    - how RepoCore works\n    - how check out works.";
	std::cout << "\n\n  In my design, RepoCore is a wrapper to make it easy to manage repository.";
	std::cout << "\n  Here, I use RepoCore to check-out some files";
	RepoCore repo(db_);
	CheckOutInfo coi;
	coi.fileName("CheckOut.cpp");
	coi.nameSpace("repository");
	coi.packageName("CheckOut");
	coi.version(1);
	CheckOut co = repo.checkOut(coi);
	std::cout << "\n\n  Before checking-out, files in '../temp/':";
	std::vector<std::string> files = FileSystem::Directory::getFiles("../temp/");
	for (auto file : files)   // clean all the file in temp folder.
		FileSystem::File::remove("../temp/" + file);
	files = FileSystem::Directory::getFiles("../temp/");
	if (files.size() == 0) std::cout << "\n    *no file*";
	else 
		for (auto file : files) 
			std::cout << "\n    - " + file;
	repo.checkOutCloneOneFile(co, "../temp/");
	std::cout << "\n\n  After checking-out one file, files in '../temp/':";
	files = FileSystem::Directory::getFiles("../temp/");
	if (files.size() == 0) std::cout << "\n    *no file*";
	else 
		for (auto file : files) 
			std::cout << "\n    - " + file;
	files = FileSystem::Directory::getFiles("../temp/");
	for (auto file : files)    // clean all the file in temp folder.
		FileSystem::File::remove("../temp/" + file);
	repo.checkOntCloneFilesWithDependency(co, "../temp/");
	std::cout << "\n\n  After checking-out files with dependency, files in '../temp/':";
	files = FileSystem::Directory::getFiles("../temp/");
	if (files.size() == 0) std::cout << "\n    *no file*";
	else 
		for (auto file : files) 
			std::cout << "\n    - " + file;
	std::cout << "\n  We can see that not only the dependency of selected file,"
		<< "\n  but also the dependency of dependency is cloned too."
		<< "\n  To prevent dead-loop in a graphy, DbWalker is used.";
	return true;
}
//----< R6 >-----------------------------------------------------------
/*
*  - show packages
*/
bool TestDb::testR6() {
	Utilities::title("Demonstrating Requirement #6");
	std::cout << "\n  The prepared RepoDb is saved in '../Repository/repoDataBase.xml'";
	DbCore<Payload> new_db;
	std::ifstream f;
	std::string xml;
	std::string line;
	f.open("../Repository/repoDataBase.xml");
	while (getline(f, line)) {
		xml.append(line);
	}
	f.close();
	Persist<Payload> persist(new_db);
	persist.fromXml(xml);
	showDb(new_db);
	Payload::showDb(new_db);
	return true;
}

int main()
{
	std::cout << "\n  Testing Repository";
	std::cout << "\n =========================";
	NoSqlDb::DbCore<Payload> db;
	TestDb tdb(db);

	tdb.invoke(&TestDb::testR1);
	tdb.invoke(&TestDb::testR2);
	tdb.invoke(&TestDb::testR3);
	tdb.invoke(&TestDb::testR4a);
	tdb.invoke(&TestDb::testR4b);
	tdb.invoke(&TestDb::testR5a);
	tdb.invoke(&TestDb::testR5b);
	tdb.invoke(&TestDb::testR6);

	std::cout << "\n\n";
    return 0;
}

