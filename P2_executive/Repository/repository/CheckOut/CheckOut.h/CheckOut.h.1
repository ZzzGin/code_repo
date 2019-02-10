#pragma once
#ifndef CHECKOUT_H
#define CHECKOUT_H

#include <string>
#include "../NoSqlDb/Executive/NoSqlDb.h"
#include "../Payload/Payload.h"
#include "../Version/Version.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"

namespace repository {
	class CheckOutInfo {
	public:
		int version() const { return version_; }
		int& version() { return version_; }
		void version(const int& version) { version_ = version; }

		std::string fileName() const { return fileName_; }
		std::string & fileName() { return fileName_; }
		void fileName(const std::string & fileName) { fileName_ = fileName; }

		std::string nameSpace() const { return nameSpace_; }
		std::string & nameSpace() { return nameSpace_; }
		void nameSpace(const std::string & nameSpace) { nameSpace_ = nameSpace; }

		std::string packageName() const { return packageName_; }
		std::string & packageName() { return packageName_; }
		void packageName(const std::string & packageName) { packageName_ = packageName; }

	private:
		int version_;
		std::string fileName_;
		std::string nameSpace_;
		std::string packageName_;

	};
	
	class CheckOut {
	public :
		CheckOut(DbCore<Payload> &db, CheckOutInfo &coi);

		std::string repoDir() const { return repoDir_; }
		std::string & repoDir() { return repoDir_; }
		void repoDir(std::string& repoDir) { repoDir_ = repoDir; }

		void clone(std::string dst);

	private:
		DbCore<Payload>& db_;
		CheckOutInfo& coi_;
		Version versionManager_;
		std::string repoDir_ = "../Repository/";

	};
}


#endif // !CHECKOUT_H
