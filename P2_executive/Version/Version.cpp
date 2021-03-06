/////////////////////////////////////////////////////////////////////
// Browse.h - Implements to browse the repository                  //
// ver 1.0                                                         //
// Jing Qi, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////
/*
* Required Files:
* ---------------
* Version.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2018
* - first release
*/

#include "Version.h"
using namespace repository;
using namespace NoSqlDb;
//----< constructor, to generate all the information >-------------------------
Version::Version(DbCore<Payload> &db, Name n, NameSpace ns) : db_(db), n_(n), ns_(ns) {
	Query<Payload> query(db_);
	Conditions<Payload> condsName;
	condsName.name("^" + n + "$");
	query.select(condsName);
	auto hasThisNameSpace = [&ns](DbElement<Payload>& elem) {
		return (elem.payLoad()).nameSpace() == ns;
	};
	for (Key key : query.keys()) {
		existedVersion_.push_back(db_[key].payLoad().version());
		if (db_[key].payLoad().status() == false)
			closedVersion_.push_back(db_[key].payLoad().version());
		else
			openedVersion_ = db_[key].payLoad().version();
	}
	if (existedVersion_.size() > 0)
		std::sort(existedVersion_.begin(), existedVersion_.end());
	if (closedVersion_.size() > 0)
		std::sort(closedVersion_.begin(), closedVersion_.end());
}

#ifdef TEST_VERSION

int main()
{
	DbCore<Payload> db;
	Payload pl;
	pl.dir("c://");
	pl.version(1);
	pl.author("demoA");
	pl.nameSpace("demoNS");
	pl.status(false);
	pl.children().push_back("demo1");
	pl.children().push_back("demo2");
	DbElement<Payload> elem;
	elem.name("demo");
	elem.descrip("demoDes");
	elem.payLoad(pl);
	db["demoNS::d1.1"] = elem;

	pl.version(2);
	pl.children().push_back("demo3");
	elem.payLoad(pl);
	db["demoNS::d1.2"] = elem;

	pl.version(3);
	pl.status(true);
	elem.payLoad(pl);
	db["demoNS::d1.3"] = elem;

	pl.version(1);
	pl.nameSpace("demoNS2");
	elem.name("demo2");
	elem.payLoad(pl);
	db["demoNS2::d2.1"] = elem;

	pl.nameSpace("demoNS");
	elem.name("demo3");
	elem.payLoad(pl);
	db["demoNS::d3.1"] = elem;

	showDb(db);
	Payload::showDb(db);

	Version v(db, "demo", "demoNS");
	std::cout << std::endl << "Current Version is:" << v.currentVersion();
	std::cout << std::endl << "Opened Version is:" << v.openedVersion();
	std::cout << std::endl;

    return 0;
}

#endif