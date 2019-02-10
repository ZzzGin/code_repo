/////////////////////////////////////////////////////////////////////
// CheckOut.h - Implements to CheckOut the repository              //
// ver 1.0                                                         //
// Jing Qi, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides two classes to meet the requirement:
*   - CheckOutInfo: this class provide the imformation to check-out
*   - Checkout:this class provide the functions to checkout
*      - CheckOut(constructor): save the checkout data
*      - cloneOneFile: clone the specific file into the dst
*      - cloneFilesWithDependency: clone all the file with dependency to the file selected
*
* Required Files:
* ---------------
* NoSqlDb.h
* Payload.h
* Version.h
* FileSystem.h
* DbWalker.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2018
* - first release
*/
#ifndef CHECKOUT_H
#define CHECKOUT_H

#include <string>
#include <vector>
#include "../NoSqlDb/Executive/NoSqlDb.h"
#include "../Payload/Payload.h"
#include "../Version/Version.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../DbWalker/DbWalker.h"

namespace repository {
	/////////////////////////////////////////////////////////////////////
	// CheckOutInfo class
	// 
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

		CheckOutInfo() = default;
		CheckOutInfo(int v, std::string f, std::string n, std::string p) {
			version_ = v;
			fileName_ = f;
			nameSpace_ = n;
			packageName_ = p;
		}

	private:
		int version_;
		std::string fileName_;
		std::string nameSpace_;
		std::string packageName_;

	};

	/////////////////////////////////////////////////////////////////////
	// CheckOut class
	// 
	class CheckOut {
	public :
		CheckOut(DbCore<Payload> &db, CheckOutInfo &coi);

		std::string repoDir() const { return repoDir_; }
		std::string & repoDir() { return repoDir_; }
		void repoDir(std::string& repoDir) { repoDir_ = repoDir; }

		bool cloneOneFile(std::string dst);
		std::vector<std::string> cloneFilesWithDependency(std::string dst);
		static void identify(std::ostream& out = std::cout);

	private:
		DbCore<Payload>& db_;
		CheckOutInfo& coi_;
		Version versionManager_;
		std::string repoDir_ = "../Repository/";

	};

	//----< show file name >---------------------------------------------
	inline void CheckOut::identify(std::ostream& out)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}
}


#endif // !CHECKOUT_H
