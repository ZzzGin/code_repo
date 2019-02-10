#pragma once
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
* NoSqlDb.h
* Payload.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2018
* - first release
*/
#ifndef BROWSE_H
#define BROWSE_H

#include <string>
#include <iostream>
//#include "../Process/Process/Process.h"
#include "../NoSqlDb/Executive/NoSqlDb.h"
#include "../Payload/Payload.h"

using namespace NoSqlDb;

namespace repository {
	/////////////////////////////////////////////////////////////////////
	// Browse class
	// 
	class Browse {
	public:
		Browse() = default;
		Browse(DbCore<Payload>& db) : db_(db) {}


		std::string appPath() const { return appPath_; }
		std::string& appPath() { return appPath_; }
		void appPath(const std::string& appPath) { appPath_ = appPath; }

		//bool displayFileInApp(std::string key, std::ostream& out = std::cout);
		bool displayFileInCMD(std::string key, std::ostream& out = std::cout);
		bool browsePackagesByQuery(Query<Payload> q, int display = 0, std::ostream& out = std::cout);
		void showBrowseHeader(std::ostream& out = std::cout);
		static void identify(std::ostream& out = std::cout);

	private:
		//Process p_;
		DbCore<Payload>& db_;
		std::string appPath_ = "c:/windows/system32/notepad.exe";
	};

	//----< show the header >---------------------------------------------
	inline void Browse::showBrowseHeader(std::ostream& out) {
		out << std::endl;
		out << std::setw(20) << std::left << "FileName";
		out << std::setw(20) << std::left << "Version";
		out << std::setw(60) << std::left << "Description";
		out << std::endl;
		out << std::setw(20) << std::left << "----------------";
		out << std::setw(20) << std::left << "----------------";
		out << std::setw(60) << std::left << 
			"----------------------------------------------------------------";
	}
	//----< show file name >---------------------------------------------

	inline void Browse::identify(std::ostream& out)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}

}


#endif