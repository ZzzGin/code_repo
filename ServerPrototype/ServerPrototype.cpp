/////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 1.0                                                             //
// Jing Qi, Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018  //
/////////////////////////////////////////////////////////////////////////

#include "ServerPrototype.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include <chrono>
#include <regex>
#include <set>

namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

//----< get files name in the repo >---------------------------------
Files Server::getFiles(const Repository::SearchPath& path)
{
  return Directory::getFiles(path);
}

//----< get directory in the repo >---------------------------------
Dirs Server::getDirs(const Repository::SearchPath& path)
{
  return Directory::getDirectories(path);
}

template<typename T>
void show(const T& t, const std::string& msg)
{
  std::cout << "\n  " << msg.c_str();
  for (auto item : t)
  {
    std::cout << "\n    " << item.c_str();
  }
}

//----< echo the message >---------------------------------
std::function<Msg(Msg, NoSqlDb::DbCore<NoSqlDb::Payload>&)> echo = [](Msg msg, NoSqlDb::DbCore<NoSqlDb::Payload>& db) {
  Msg reply = msg;
  reply.to(msg.from());
  reply.from(msg.to());
  return reply;
};

//----< processing the msg of getFiles>---------------------------------
std::function<Msg(Msg, NoSqlDb::DbCore<NoSqlDb::Payload>&)> getFiles = [](Msg msg, NoSqlDb::DbCore<NoSqlDb::Payload>& db) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getFiles");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files files = Server::getFiles(searchPath);
    size_t count = 0;
    for (auto item : files)
    {
      std::string countStr = Utilities::Converter<size_t>::toString(++count);
      reply.attribute("file" + countStr, item);
    }
  }
  else
  {
    std::cout << "\n  getFiles message did not define a path attribute";
  }
  return reply;
};

//----< processing the msg of getDirs>---------------------------------
std::function<Msg(Msg, NoSqlDb::DbCore<NoSqlDb::Payload>&)> getDirs = [](Msg msg, NoSqlDb::DbCore<NoSqlDb::Payload>& db) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getDirs");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files dirs = Server::getDirs(searchPath);
    size_t count = 0;
    for (auto item : dirs)
    {
      if (item != ".." && item != ".")
      {
        std::string countStr = Utilities::Converter<size_t>::toString(++count);
        reply.attribute("dir" + countStr, item);
      }
    }
  }
  else
  {
    std::cout << "\n  getDirs message did not define a path attribute";
  }
  return reply;
};

//----< processing the msg of testComm>---------------------------------
std::function<Msg(Msg, NoSqlDb::DbCore<NoSqlDb::Payload>&)> testComm = [](Msg msg, NoSqlDb::DbCore<NoSqlDb::Payload>& db) {
	std::string tempDir = msg.from().address+std::to_string(msg.from().port);
	FileSystem::Directory::setCurrentDirectory("../Repository/temp/");
	if (!FileSystem::Directory::exists(tempDir))
		FileSystem::Directory::create(tempDir);
	FileSystem::Directory::setCurrentDirectory("../");
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("testComm");
	return reply;
};

//----< processing the msg of needFile>---------------------------------
std::function<Msg(Msg, NoSqlDb::DbCore<NoSqlDb::Payload>&)> needFile = [](Msg msg, NoSqlDb::DbCore<NoSqlDb::Payload>& db) {
	Msg reply;
	//msg.show();
	std::string filename = msg.value("fileName");
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("transferFile");
	reply.file(filename);
	reply.attribute("fromDir", msg.value("fromDir"));
	reply.attribute("toDir", msg.value("toDir"));
	return reply;
};

//----< processing the msg of CheckIn>---------------------------------
std::function<Msg(Msg, NoSqlDb::DbCore<NoSqlDb::Payload>&)> CheckIn = [](Msg msg, NoSqlDb::DbCore<NoSqlDb::Payload>& db) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("checkinResp");
	std::string errMessage = " > No Error. Check-In accepted.";
	// collect info from message
	std::string filename = msg.value("fileName");
	std::string nameSpace = msg.value("nameSpace");
	std::string packageName = msg.value("packageName");
	std::string author = msg.value("author");
	std::string description = msg.value("description");
	std::string close = msg.value("close");
	std::vector<std::string> children_vector;
	std::set<std::string> children_set;
	for (auto k : msg.keys()) {
		if (k.find("children") != std::string::npos) {
			children_set.insert(msg.value(k));
		}
	}
	for (auto c : children_set) {
		children_vector.push_back(c);
		if (db.dbStore()[c].payLoad().status() == 1) {
			errMessage = " > Children is not closed. Check-In rejected.";
			reply.attribute("err", errMessage);
			return reply;
		}
	}


	// do something to checkin
	repository::CheckInInfo cii(author, description, filename, nameSpace, packageName);
	cii.children(children_vector);
	repository::CheckIn ci(db, cii);
	Sleep(100);
	ci.submitFile("../Repository/temp/" + msg.from().address + std::to_string(msg.from().port) + "/" + filename);
	if (close == "True") {
		ci.close();
	}

	reply.attribute("err", errMessage);
	return reply;
};

//----< processing the msg of CheckOut>---------------------------------
std::function<Msg(Msg, NoSqlDb::DbCore<NoSqlDb::Payload>&)> CheckOut = [](Msg msg, NoSqlDb::DbCore<NoSqlDb::Payload>& db) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("checkoutResp");
	std::string errMsg = " > No error. Check out Successfully.";
	// do something to checkout
	std::string key = msg.value("key");
	if (!db.contains(key)) {
		errMsg = " > Key not found. Check out rejected.";
		reply.attribute("err", errMsg);
		reply.attribute("checkoutFile", "nothing");
		return reply;
	}
	repository::CheckOutInfo coi(db.dbStore()[key].payLoad().version(),
		db.dbStore()[key].name(),
		db.dbStore()[key].payLoad().nameSpace(),
		db.dbStore()[key].payLoad().packageName());
	repository::CheckOut co(db, coi);
	if (msg.value("dependency") == "True") {
		std::vector<std::string> filelist = 
			co.cloneFilesWithDependency("../Repository/temp/" + msg.from().address + std::to_string(msg.from().port) + "/");
		size_t count = 0;
		for (auto f : filelist) {
			std::cout << "\n" << f;
			std::string countStr = Utilities::Converter<size_t>::toString(++count);
			reply.attribute("checkoutFile" + countStr, db.dbStore()[f].name());
		}
		reply.attribute("checkoutFile", db.dbStore()[key].name());
	}
	else {
		co.cloneOneFile("../Repository/temp/" + msg.from().address + std::to_string(msg.from().port) + "/");
		reply.attribute("checkoutFile", db.dbStore()[key].name());
	}
	reply.attribute("err", errMsg);
	return reply;
};

//----< processing the msg of Browse>---------------------------------
std::function<Msg(Msg, NoSqlDb::DbCore<NoSqlDb::Payload>&)> browseSearch = [](Msg msg, NoSqlDb::DbCore<NoSqlDb::Payload>& db) {
	Msg reply;
	std::string nameSpace = msg.value("NameSpace");
	std::string packageName = msg.value("PackageName");
	std::string fileName = msg.value("FileName");
	std::string dependency = msg.value("Dependency");
	int versionNumber = -1;
	if (msg.value("VersionNumber") != "")
		versionNumber = std::stoi(msg.value("VersionNumber"));

	Query<Payload> q(db);
	auto isItGood = [&nameSpace, &packageName, &fileName, &versionNumber, &dependency](DbElement<Payload>& elem) {
		bool result = true;
		if (nameSpace != "") 
			if (elem.payLoad().nameSpace() != nameSpace) result = false;
		if (packageName != "") 
			if (elem.payLoad().packageName() != packageName)result = false;
		if (fileName != "") 
			if (elem.name() != fileName)result = false;
		if (dependency != "") {
			std::vector<std::string> c = elem.payLoad().children();
			if (std::find(c.begin(), c.end(), dependency)==c.end()) result = false;
		}
		if (versionNumber != -1) 
			if (elem.payLoad().version() != versionNumber)result = false;
		return result;
	};
	q.select(isItGood);
	std::cout << "\n  Keys selected:";
	size_t count = 0;
	for (auto k : q.keys()) {
		std::cout << "\n  - " << k ;
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("result" + countStr, k);
	}
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("browseSuccess");
	return reply;
};

//----< processing the msg of ViewMetadata>---------------------------------
std::function<Msg(Msg, NoSqlDb::DbCore<NoSqlDb::Payload>&)> ViewMetadata = [](Msg msg, NoSqlDb::DbCore<NoSqlDb::Payload>& db) {
	Msg reply;
	std::string key = msg.value("key");
	// do something to checkin
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("viewMetadataSuccess");
	if (!db.contains(key)) {
		reply.attribute("haveThisKey", "false");
		return reply;
	}
	reply.attribute("dateTime", db.dbStore()[key].dateTime());
	reply.attribute("packageName", db.dbStore()[key].payLoad().packageName());
	reply.attribute("description", db.dbStore()[key].descrip());
	reply.attribute("name", db.dbStore()[key].name());
	reply.attribute("author", db.dbStore()[key].payLoad().author());
	reply.attribute("direction", db.dbStore()[key].payLoad().dir());
	reply.attribute("nameSpace", db.dbStore()[key].payLoad().nameSpace());
	reply.attribute("status", db.dbStore()[key].payLoad().status() ? "true" : "false");
	reply.attribute("version", std::to_string(db.dbStore()[key].payLoad().version()));
	reply.attribute("keyName", key);
	reply.attribute("haveThisKey", "true");
	size_t count = 0;
	for (auto c : db.dbStore()[key].payLoad().children()) {
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("children" + countStr, c);
	}
	return reply;
};

//----< processing the msg of browseDependency>---------------------------------
std::function<Msg(Msg, NoSqlDb::DbCore<NoSqlDb::Payload>&)> browseDependency = [](Msg msg, NoSqlDb::DbCore<NoSqlDb::Payload>& db) {
	Msg reply;
	std::vector<std::string> dep;
	if (msg.value("isRecursive") == "false") {
		dep = db.dbStore()[msg.value("key")].payLoad().children();
	}
	else {
		repository::DbWalker w(db);
		dep = w.dependency(msg.value("key"));
	}
	std::cout << "\n  Dependent Keys are:";
	size_t count = 0;
	for (auto d : dep) {
		std::cout << "\n  - " << d;
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("result" + countStr, d);
	}
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("browseDependencySuccess");
	return reply;
};

//----< processing the msg of browseRESearch>---------------------------------
std::function<Msg(Msg, NoSqlDb::DbCore<NoSqlDb::Payload>&)> browseRESearch = [](Msg msg, NoSqlDb::DbCore<NoSqlDb::Payload>& db) {
	Msg reply;
	std::string nameSpace = msg.value("NameSpace");
	if (nameSpace == "")nameSpace = ".*";
	std::string packageName = msg.value("PackageName");
	if (packageName == "")packageName = ".*";
	std::string fileName = msg.value("FileName");
	if (fileName == "")fileName = ".*";
	std::string dependency = msg.value("Dependency");
	if (dependency == "")dependency = ".*";
	std::string versionNumber = msg.value("VersionNumber");
	if (versionNumber == "")versionNumber = ".*";
	Query<Payload> q(db);
	auto isItGood = [&nameSpace, &packageName, &fileName, &versionNumber, &dependency](DbElement<Payload>& elem) {
		std::string ns = elem.payLoad().nameSpace();
		std::string pn = elem.payLoad().packageName();
		std::string fn = elem.name();
		std::vector<std::string> c = elem.payLoad().children();
		std::string vn = std::to_string(elem.payLoad().version());
		std::regex e_ns(nameSpace);
		std::regex e_pn(packageName);
		std::regex e_fn(fileName);
		std::regex e_d(dependency);
		std::regex e_vn(versionNumber);
		bool result = true;
		if (!std::regex_match(ns, e_ns)) result = false;
		if (!std::regex_match(pn, e_pn)) result = false;
		if (!std::regex_match(fn, e_fn)) result = false;
		if (!std::regex_match(vn, e_vn)) result = false;
		bool c_result = false;
		for (auto _c : c) {
			if (std::regex_match(_c, e_d)) {
				c_result = true;
				break;
			}
		}
		return result && c_result;
	};
	q.select(isItGood);
	std::cout << "\n  Keys selected:";
	size_t count = 0;
	for (auto k : q.keys()) {
		std::cout << "\n  - " << k;
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("result" + countStr, k);
	}
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("browseSuccess");
	return reply;
};


//----< processing the msg of needCheckOutFile>---------------------------------
std::function<Msg(Msg, NoSqlDb::DbCore<NoSqlDb::Payload>&)> needCheckOutFile = [](Msg msg, NoSqlDb::DbCore<NoSqlDb::Payload>& db) {
	Msg reply;
	std::string filename = msg.value("fileName");
	reply.to(msg.from());
	reply.from(msg.to());
	reply.file(filename);
	reply.command("fileTransfer");
	reply.attribute("fromDir", "../Repository/temp/" + msg.from().address + std::to_string(msg.from().port) + "/");
	reply.attribute("toDir",msg.value("toDir"));
	return reply;
};

int main()
{
  std::cout << "\n  Repository Server initiating..";
  std::cout << "\n ==========================";
  std::cout << "\n";
  //StaticLogger<1>::attach(&std::cout);
  //StaticLogger<1>::start();
  std::cout << "\n    > Loading Database..";
  NoSqlDb::DbCore<Payload> db;
  std::ifstream f;
  std::string xml;
  std::string line;
  f.open("../Repository/repoDataBase.xml");
  while (getline(f, line)) {
	  xml.append(line);
  }
  f.close();
  Persist<Payload> persist(db);
  persist.fromXml(xml);
  showDb(db);
  Payload::showDb(db);
  std::cout << "\n    > Database loaded.";

  Server server(serverEndPoint, "ServerPrototype", db);
  server.start();
  server.addMsgProc("echo", echo);
  server.addMsgProc("getFiles", getFiles);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("serverQuit", echo);
  server.addMsgProc("testComm", testComm);
  server.addMsgProc("needFile", needFile);
  server.addMsgProc("checkin", CheckIn);
  server.addMsgProc("checkout", CheckOut);
  server.addMsgProc("browseSearch", browseSearch);
  server.addMsgProc("vm", ViewMetadata);
  server.addMsgProc("browseDependency", browseDependency);
  server.addMsgProc("browseRESearch", browseRESearch);
  server.addMsgProc("needCheckOutFile", needCheckOutFile);
  server.processMessages();
  std::cout << "\n    > Server started.";
  
  Msg msg(serverEndPoint, serverEndPoint);  // send to self
  std::cout << "\n    > press enter to exit";
  std::cin.get();
  std::cout << "\n";
  msg.command("serverQuit");
  server.postMessage(msg);
  server.stop();
  return 0;
}

