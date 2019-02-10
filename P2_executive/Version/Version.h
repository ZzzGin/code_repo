#pragma once
/////////////////////////////////////////////////////////////////////
// Version.h - Implements to manage the versions                   //
// ver 1.0                                                         //
// Jing Qi, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provide the functions to manage the version
    - currentVersion: if the newest record is open, return the open version #
	                  if it is closed, return the # next to the max version#
    - getClosedVersionNumbers: return a vector contains the Version # closed.
	- getExistedVersionNumbers: return a vector contains the version # exist.
	- openedVersion: return the version # opened. If there is no open version, return 0.
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
#ifndef VERSION_H
#define VERSION_H

#include "../NoSqlDb/Executive/NoSqlDb.h"
#include "../Payload/Payload.h"

using namespace NoSqlDb;
namespace repository {
	/////////////////////////////////////////////////////////////////////
	// repository class
	// 
	class Version {
	public:
		using Name = std::string;
		using NameSpace = std::string;
		Version(DbCore<Payload> &db, Name n, NameSpace ns);

		DbCore<Payload> db() const { return db_; }
		DbCore<Payload>&db() { return db_; }
		void db(DbCore<Payload>& db) { db_ = db; }

		inline int currentVersion() { return closedVersion_.size() + 1; }
		inline std::vector<int> getClosedVersionNumbers() { return closedVersion_; }
		inline std::vector<int> getExistedVersionNumbers() { return existedVersion_; }
		inline int openedVersion() { return openedVersion_; }
		static void identify(std::ostream& out = std::cout);

	private:
		Name n_;
		NameSpace ns_;
		DbCore<Payload> db_;
		std::vector<int> existedVersion_;
		std::vector<int> closedVersion_;
		int openedVersion_ = 0;
	};
	//----< show the filename >---------------------------------------------
	inline void Version::identify(std::ostream& out)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}
}

#endif 