/////////////////////////////////////////////////////////////////////
// DbWalker.cpp - Implements to search the dependency              //
// ver 1.0                                                         //
// Jing Qi, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////
/*
*
* Required Files:
* ---------------
* DbWalker.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2018
* - first release
*/
#include "DbWalker.h"

using namespace NoSqlDb;
using namespace repository;
//----< return a list of keys in dependance >----------------------------
std::vector<std::string> DbWalker::dependency(std::string root) {
	recursiveWalker(root);
	std::vector<std::string> results;
	std::copy(passedItems_.begin(), passedItems_.end(), std::back_inserter(results));
	return results;
}
//----< the recursive part of searching >--------------------------------
void DbWalker::recursiveWalker(std::string root) {
	for (std::string key : db_[root].payLoad().children()) {
		Dependancies::const_iterator got = passedItems_.find(key);
		if (got == passedItems_.end()) {
			passedItems_.insert(key);
			recursiveWalker(key);
		}
		else return;
	}
}

#ifdef TEST_DBWALKER
int main()
{
	DbCore<Payload> db;
	// 5-> 4-> 1-> 2-> 3-> 4-> 1-> 2-> 3-> 4->......
	Payload pl1;
	pl1.children().push_back("2");
	Payload pl2;
	pl2.children().push_back("3");
	Payload pl3;
	pl3.children().push_back("4");
	Payload pl4;
	pl4.children().push_back("1");
	DbElement<Payload> e1;
	e1.payLoad(pl1);
	db["1"] = e1;
	DbElement<Payload> e2;
	e2.payLoad(pl2);
	db["2"] = e2;
	DbElement<Payload> e3;
	e3.payLoad(pl3);
	db["3"] = e3;
	DbElement<Payload> e4;
	e4.payLoad(pl4);
	db["4"] = e4;
	DbElement<Payload> e5;
	e5.payLoad(pl3);
	db["5"] = e5;

	showDb(db);
	Payload::showDb(db);

	DbWalker w(db);
	std::vector<std::string> dep;
	dep = w.dependency("5");

	std::cout << std::endl << " Here, we define a database, the dependency of each item is:";
	std::cout << std::endl << "   5-> 4-> 1-> 2-> 3-> 4-> 1-> 2-> 3-> 4->......";
	std::cout << std::endl << " 5 has " << dep.size() << " dependencies" << std::endl;
	std::cout << " They are:" << std::endl;
	for (auto key : dep) {
		std::cout << "    -> " << key << std::endl;
	}
	std::cout << std::endl;
    return 0;
}

#endif