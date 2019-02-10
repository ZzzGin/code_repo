#pragma once
/////////////////////////////////////////////////////////////////////
// CheckIn.h - Implements to Check-in the repository               //
// ver 1.0                                                         //
// Jing Qi, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides two classes to meet the requirement:
*   - CheckInInfo: this class provide the imformation to check-in
*   - CheckIn:this class provide the functions to checkin
*      - CheckIn(constructor): check the validity of the item;
*                              add the item into the db;
*      - close: close the checkin, no change can be made after it
*      - changeMetaData: modify the metaData of the opened item
*      - submitFile: submit a File to the repository
*                              
*
* Required Files:
* ---------------
* NoSqlDb.h
* Payload.h
* Version.h
* FileSystem.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2018
* - first release
*/
#ifndef CHECKIN_H
#define CHECKIN_H

#include <string>
#include <vector>
#include <iostream>
#include "../NoSqlDb/Executive/NoSqlDb.h"
#include "../Payload/Payload.h"
#include "../Version/Version.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"

using namespace NoSqlDb;

namespace repository {
	/////////////////////////////////////////////////////////////////////
	// CheckInInfo class
	// - provide the information to checkin
	class CheckInInfo {
	public:
		std::string filename() const { return filename_; }
		std::string & filename() { return filename_; }
		void filename(const std::string & filename) { filename_ = filename; }

		std::string discription() const { return discription_; }
		std::string & discription() { return discription_; }
		void discription(const std::string & discription) { discription_ = discription; }

		std::string nameSpace() const { return namespace_; }
		std::string & nameSpace() { return namespace_; }
		void nameSpace(const std::string & nameSpace) { namespace_ = nameSpace; }

		std::string author() const { return author_; }
		std::string & author() { return author_; }
		void author(const std::string & author) { author_ = author; }

		std::string package() const { return package_; }
		std::string & package() { return package_; }
		void package(const std::string & package) { package_ = package; }

		std::vector<std::string> children()const { return children_; }
		std::vector<std::string> & children() { return children_; }
		void children(const std::vector<std::string> & children) { children_ = children; }

		CheckInInfo() = default;
		CheckInInfo(std::string a, std::string d, std::string f,
			std::string n, std::string p) {
			author(a);
			discription(d);
			filename(f);
			nameSpace(n);
			package(p);
		}

	private:
		std::string filename_;
		std::string discription_;
		std::string namespace_;
		std::string author_;
		std::string package_;
		std::vector<std::string> children_;
	};
	/////////////////////////////////////////////////////////////////////
	// CheckIn class
	// - provide the functions of checkin
	class CheckIn {
	public:
		using FileName = std::string;

		std::string repoDir() const { return repoDir_; }
		std::string & repoDir() { return repoDir_; }
		void repoDir(std::string& repoDir) { repoDir_ = repoDir; }

		CheckIn(DbCore<Payload> &db, CheckInInfo& cii);
		void close();
		void changeMetaData(CheckInInfo& cii);
		void submitFile(std::string dirAndFile);
		static void identify(std::ostream& out = std::cout);

	private:
		DbCore<Payload>& db_;
		CheckInInfo& cii_;
		std::string opened_key_;
		std::string repoDir_ = "../Repository/";
		std::string dir_;
		Version versionManager_;
		Payload pl_;
		DbElement<Payload> elem_;
		bool open_status_ = true;
	};
	inline void CheckIn::identify(std::ostream& out)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}
}

#endif 