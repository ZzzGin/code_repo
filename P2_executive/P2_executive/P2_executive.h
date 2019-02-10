#pragma once
/////////////////////////////////////////////////////////////////////
// P2_executive.h                                                  //
// ver 1.0                                                         //
// Jing Qi, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This file works to support the executive program
*
* Required Files:
* ---------------
* Browse.h
* CheckIn.h
* CheckOut.h
* RepositoryCore.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2018
* - first release
*/
#include <string>
#include "../Browse/Browse.h"
#include "../CheckIn/CheckIn.h"
#include "../CheckOut/CheckOut.h"
#include "../RepositoryCore/RepositoryCore.h"

using namespace NoSqlDb;
using namespace repository;

class TestDb
{
public:
	using MPtr = bool(TestDb::*)();

	TestDb(NoSqlDb::DbCore<NoSqlDb::Payload>& db) : db_(db) {}
	static void identify(std::ostream& out = std::cout);

	bool invoke(MPtr mPtr);
	bool testR1();
	bool testR2();
	bool testR3();
	bool testR4a();
	bool testR4b();
	bool testR5a();
	bool testR5b();
	bool testR6();

private:
	NoSqlDb::DbCore<NoSqlDb::Payload>& db_;
};

//----< show file name >---------------------------------------------
inline void TestDb::identify(std::ostream& out)
{
	out << "\n  \"" << __FILE__ << "\"";
}
//----< invoke the test >---------------------------------------------
inline bool TestDb::invoke(TestDb::MPtr mPtr)
{
	try {
		bool result = ((*this).*mPtr)();
		if (result)
		{
			std::cout << "\npassed <<<<\n";
		}
		else
		{
			std::cout << "\nfailed <<<<\n";
		}
		return result;
	}
	catch (std::exception& ex)
	{
		std::cout << "\n    exception thrown:";
		std::cout << "\n    " << ex.what();
		std::cout << "\nfailed <<<<\n";
		return false;
	}
}