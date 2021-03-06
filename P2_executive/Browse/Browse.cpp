/////////////////////////////////////////////////////////////////////
// Browse.h - Implements to browse the repository                  //
// ver 1.0                                                         //
// Jing Qi, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provide the functions of browsing the repository:
*   -displayFileInCMD: show the file in the cmd window;
*   -browsePackagesByQuery: show the file queried
*   -showBrowseHeader: show the header
*
* Required Files:
* ---------------
*  Browse.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2018
* - first release
*/


#include "Browse.h"
#include <fstream>
#include "../Process/Process/Process.h"

using namespace repository;
using namespace NoSqlDb;

//bool Browse::displayFileInApp(std::string key, std::ostream& out) {
//	try {
//		if (!db_.contains(key))return false;
//		p_.title("Browsing file");
//		p_.application(appPath_);
//		p_.commandLine("/A " + db_[key].payLoad().dir() + db_[key].name()+
//			"." + std::to_string(db_[key].payLoad().version()));
//
//		out << "\n  Browsing: \"" << db_[key].payLoad().dir() + db_[key].name() << "\"";
//		out << "\n  With: \"" << appPath_ << "\"";
//		p_.create();
//
//		return true;
//	}
//	catch (const std::exception& e) {
//		out << e.what();
//		return false;
//	}
//}

//----< display the file by key in CMD window >---------------------------------------------
bool Browse::displayFileInCMD(std::string key, std::ostream& out) {
	try {
		if (!db_.contains(key))return false;
		std::ifstream ifs;
		std::string contents;
		std::string i_line;
		ifs.open(db_[key].payLoad().dir() + db_[key].name() +
			"." + std::to_string(db_[key].payLoad().version()));
		while (getline(ifs, i_line)) {
			contents.append(i_line + "\n");
		}
		out << std::endl << contents;
		return true;
	}
	catch (const std::exception& e) {
		out << e.what();
		return false;
	}
}
//----< show the file by query(display==1 show the files content, ==0 don't show content) >-------------
bool Browse::browsePackagesByQuery(Query<Payload> q, int display, std::ostream& out) {
	showBrowseHeader(out);
	if (q.keys().size() == 0) {
		out << std::endl << "  - No Key is selected.";
		return false;
	}
	for (auto key : q.keys()) {
		out << std::endl;
		out << std::setw(20) << std::left << db_[key].name().substr(0, 18);
		out << std::setw(20) << std::left << std::to_string(db_[key].payLoad().version()).substr(0, 18);
		out << std::setw(60) << std::left << db_[key].descrip().substr(0, 78);
	}
	for (auto key : q.keys()) {
		if (display == 1) { 
			out << std::endl << "  Browsing File: " + db_[key].name() + "\n";
			displayFileInCMD(key); 
		}
		//else if (display == 2) displayFileInApp(key);
		/*else if (display == 3) {
			displayFileInCMD(key);
			displayFileInApp(key);
		}*/
	}
	return true;
}

#ifdef TEST_BROWSE
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

	//browse teh iPayload
	Browse b(db);
	b.displayFileInApp("NoSqlDb::IPayload.h.1");
	b.displayFileInCMD("NoSqlDb::IPayload.h.1");
	std::cout << std::endl;

	//browse the query
	Query<Payload> q(db);
	std::string n = "NoSqlDb";
	auto isNameSpace = [&n](DbElement<Payload>& elem) {
		return (elem.payLoad()).nameSpace() == n;
	};
	q.select(isNameSpace);
	b.browsePackagesByQuery(q, 1);

	std::cout << std::endl;
	return 0;
}
#endif

