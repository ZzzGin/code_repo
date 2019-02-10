#pragma once
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

	private:
		std::string filename_;
		std::string discription_;
		std::string namespace_;
		std::string author_;
		std::string package_;
		std::vector<std::string> children_;
	};

	class CheckIn {
	public:
		using FileName = std::string;

		std::string repoDir() const { return repoDir_; }
		std::string & repoDir() { return repoDir_; }
		void repoDir(std::string& repoDir) { repoDir_ = repoDir; }

		CheckIn(DbCore<Payload> &db, CheckInInfo& cii);
		void close();
		void changeMetaData(CheckInInfo& cii);
		void submitFile(std::string dir);

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
	
}

#endif 