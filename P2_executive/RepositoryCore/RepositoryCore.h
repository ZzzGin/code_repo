#pragma once
/////////////////////////////////////////////////////////////////////
// RepositoryCore.h - eazy ways to manage the repo                 //
// ver 1.0                                                         //
// Jing Qi, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package create a class works as a wrapper for the managements to a repository.
*    - checkIn: create a CheckIn class instance to manage checkin
*    - checkInClose: closs a CheckIn
*    - checkInMetadataModify: modify the metadata
*    - checkInSubmitFile: submit a file to the repo
*    - CheckOut checkOut>: create a CheckOut Class instance to manage checkout
*    - checkOutCloneOneFile: clone one file by the key
*    - checkOntCloneFilesWithDependency: clone all the file in dependency
*    - browse(): create a browse class instance to manage browsing
*    - browseFileInCMD: display the file content in CMD
*    - browsePackagesByQuery: display the file content by query

* Required Files:
* ---------------
* CheckIn.h
* CheckOut.h
* Browse.h
* Version.h
* Process.h
* FileSystem.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2018
* - first release
*/
#ifndef REPOSITORYCORE_H
#define REPOSITORYCORE_H

#include <string>
#include "../CheckIn/CheckIn.h"
#include "../CheckOut/CheckOut.h"
#include "../Browse/Browse.h"
#include "../Version/Version.h"
#include "../Process/Process/Process.h" 
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"

using namespace NoSqlDb;

namespace repository {
	/////////////////////////////////////////////////////////////////////
	// RepoCore class
	// 
	class RepoCore {
	public:
		std::string repoDir() const { return repoDir_; }
		std::string & repoDir() { return repoDir_; }
		void repoDir(std::string& repoDir) { repoDir_ = repoDir; }

		RepoCore(DbCore<Payload>& db):db_(db){}

		CheckIn checkIn(CheckInInfo &cii);
		void checkInClose(CheckIn& ci);
		void checkInMetadataModify(CheckIn& ci, CheckInInfo &cii);
		void checkInSubmitFile(CheckIn& ci, std::string dirAndFile);

		CheckOut checkOut(CheckOutInfo &coi);
		void checkOutCloneOneFile(CheckOut& co, std::string dst);
		void checkOntCloneFilesWithDependency(CheckOut& co, std::string dst);

		Browse browse();
		//bool browseFileInApp(Browse& b, std::string key, std::ostream& out = std::cout);
		bool browseFileInCMD(Browse& b, std::string key, std::ostream& out = std::cout);
		bool browsePackagesByQuery(Browse& b, Query<Payload> q, 
			int display = 0, std::ostream& out = std::cout);
		static void identify(std::ostream& out = std::cout);

	private:
		DbCore<Payload>& db_;
		std::string repoDir_ = "../Repository/";
	};
	//----< check in, return a CheckIn instance>--------------------------------------
	inline CheckIn RepoCore::checkIn(CheckInInfo &cii) {
		CheckIn ci(db_, cii);
		return ci;
	}
	//----< close a check in >---------------------------------------------
	inline void RepoCore::checkInClose(CheckIn& ci) {
		ci.close();
	}
	//----< modify the metadata >---------------------------------------------
	inline void RepoCore::checkInMetadataModify(CheckIn& ci, CheckInInfo &cii) {
		ci.changeMetaData(cii);
	}
	//----< submit a file >---------------------------------------------
	inline void RepoCore::checkInSubmitFile(CheckIn& ci, std::string dirAndFile) {
		ci.submitFile(dirAndFile);
	}
	//----< checkout, return a CheckOut instance >------------------------
	inline CheckOut RepoCore::checkOut(CheckOutInfo &coi) {
		CheckOut co(db_, coi);
		return co;
	}
	//----< check out one file >---------------------------------------------
	inline void RepoCore::checkOutCloneOneFile(CheckOut& co, std::string dst) {
		co.cloneOneFile(dst);
	}
	//----< check out all files with dependency >---------------------------------------------
	inline void RepoCore::checkOntCloneFilesWithDependency(CheckOut& co, std::string dst) {
		co.cloneFilesWithDependency(dst);
	}
	//----< manage the browse >---------------------------------------------
	inline Browse RepoCore::browse() {
		Browse b(db_);
		return b;
	}

	/*inline bool RepoCore::browseFileInApp(Browse& b, std::string key, std::ostream& out) {
		return b.displayFileInApp(key, out);
	}*/
	//----< display the file in cmd window >---------------------------------------------
	inline bool RepoCore::browseFileInCMD(Browse& b, std::string key, std::ostream& out) {
		return b.displayFileInCMD(key, out);
	}
	//----< display the file by query >---------------------------------------------
	inline bool RepoCore::browsePackagesByQuery(Browse& b, Query<Payload> q,
		int display, std::ostream& out) {
		return b.browsePackagesByQuery(q, display, out);
	}
	//----< show the filename >---------------------------------------------
	inline void RepoCore::identify(std::ostream& out)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}

}


#endif