#pragma once
/////////////////////////////////////////////////////////////////////
// DbWalker.h - Implements to search the dependency                //
// ver 1.0                                                         //
// Jing Qi, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provide the functions of browsing the repository:
*   -dependency: return a list of keys which depend on the key selected.
* Here, we use the unordered_set, whose compelicity of searching is O(n)
*
* Required Files:
* ---------------
* NoSqlDb.h
* Payload.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2018
* - first release
*/
#ifndef DBWALKER_H
#define DBWALKER_H

#include "../NoSqlDb/Executive/NoSqlDb.h"
#include "../Payload/Payload.h"
#include <unordered_set>

using namespace NoSqlDb;

namespace repository {
	/////////////////////////////////////////////////////////////////////
	// DbWalker class
	// 
	class DbWalker {
	public:
		using Dependancies = std::unordered_set<std::string>;
		DbWalker(DbCore<Payload>& db):db_(db){}
		std::vector<std::string> dependency(std::string root);
		static void identify(std::ostream& out = std::cout);

	private:
		DbCore<Payload>& db_;
		Dependancies passedItems_;
		void recursiveWalker(std::string root);
	};
	inline void DbWalker::identify(std::ostream& out)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}
}

#endif
