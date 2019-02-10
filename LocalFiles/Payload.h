#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <string>
#include <vector>
#include <iostream>
#include "../NoSqlDb/XmlDocument/XmlDocument/XmlDocument.h"
#include "../NoSqlDb/XmlDocument/XmlElement/XmlElement.h"
#include "../NoSqlDb/DbCore/Definitions.h"
#include "../NoSqlDb/DbCore/DbCore.h"
#include "IPayload.h"

namespace NoSqlDb {
	class Payload : public IPayLoad<Payload> {
	public:
		Payload() = default;
		Payload(const std::string& val) : dir_(val) {}
		static void identify(std::ostream& out = std::cout);
		Payload& operator=(const std::string& val)
		{
			dir_ = val;
			return *this;
		}
		operator std::string() { return dir_; }

		std::string dir() const { return dir_; }
		std::string& dir() { return dir_; }
		void dir(const std::string& dir) { dir_ = dir; }

		std::string author() const { return author_; }
		std::string& author() { return author_; }
		void author(const std::string & author) { author_ = author; }

		std::string nameSpace() const { return nameSpace_; }
		std::string& nameSpace() { return nameSpace_; }
		void nameSpace(const std::string & nameSpace) { nameSpace_ = nameSpace; }

		std::string packageName() const { return packageName_; }
		std::string& packageName() { return packageName_; }
		void packageName(const std::string & packageName) { packageName_ = packageName; }

		bool status() const { return status_; }
		bool &status() { return status_; }
		void status(const bool &status) { status_ = status; }

		int version() const { return version_; }
		int &version() { return version_; }
		void version(const int &version) { version_ = version; }

		std::vector<std::string>& children() { return children_; }
		std::vector<std::string> children() const { return children_; }
		void children(std::vector<std::string> children) { children_ = children; }

		bool hasChild(const std::string & c) {
			return std::find(children_.begin(), children_.end(), c) != children_.end();
		}

		Sptr toXmlElement();
		static Payload fromXmlElement(Sptr elem);

		static void showPayLoadHeaders(std::ostream& out = std::cout);
		static void showElementPayLoad(NoSqlDb::DbElement<Payload>& elem, std::ostream& out = std::cout);
		static void showDb(NoSqlDb::DbCore<Payload>& db, std::ostream& out = std::cout);

	private:
		std::string dir_;
		std::string author_;
		std::string nameSpace_;
		std::string packageName_;
		bool status_;
		int version_ = 0;
		std::vector<std::string> children_;

	};

	//----< show file name >---------------------------------------------

	inline void Payload::identify(std::ostream& out)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}

	//----< create XmlElement that represents PayLoad instance >---------
	/*
	* - Required by Persist<PayLoad>
	*/
	inline Sptr Payload::toXmlElement() {
		Sptr sPtr = XmlProcessing::makeTaggedElement("payload");
		XmlProcessing::XmlDocument doc(makeDocElement(sPtr));
		Sptr sPtrDir = XmlProcessing::makeTaggedElement("dir", dir_);
		sPtr->addChild(sPtrDir);

		Sptr sPtrAut = XmlProcessing::makeTaggedElement("author", author_);
		sPtr->addChild(sPtrAut);

		Sptr sPtrNam = XmlProcessing::makeTaggedElement("nameSpace", nameSpace_);
		sPtr->addChild(sPtrNam);

		Sptr sPtrVer = XmlProcessing::makeTaggedElement("version", std::to_string(version_));
		sPtr->addChild(sPtrVer);

		Sptr sPtrStat = XmlProcessing::makeTaggedElement("status", std::to_string(status_));
		sPtr->addChild(sPtrStat);

		Sptr sPtrChilren = XmlProcessing::makeTaggedElement("children");
		sPtr->addChild(sPtrChilren);
		for (auto child : children_) {
			Sptr sPtrChild = XmlProcessing::makeTaggedElement("child", child);
			sPtrChilren->addChild(sPtrChild);
		}
		return sPtr;
	}

	//----< create PayLoad instance from XmlElement >--------------------
	/*
	* - Required by Persist<PayLoad>
	*/
	inline Payload Payload::fromXmlElement(Sptr pElem) {
		Payload pl;
		for (auto pChild : pElem->children()) {
			std::string tag = pChild->tag();
			std::string val = pChild->children()[0]->value();
			if (tag == "dir")
				pl.dir(val);
			else if (tag == "author")
				pl.author(val);
			else if (tag == "nameSpace")
				pl.nameSpace(val);
			else if (tag == "version")
				pl.version(std::stoi(val));
			else if (tag == "status")
				pl.status((val == "1") ? true : false);
			else if (tag == "children") {
				std::vector<Sptr> pFileChildren = pChild->children();
				for (auto pFileChild : pFileChildren) {
					pl.children_.push_back(pFileChild->children()[0]->value());
				}
			}
		}
		return pl;
	}

	/////////////////////////////////////////////////////////////////////
	// PayLoad display functions

	inline void Payload::showPayLoadHeaders(std::ostream& out)
	{
		out << "\n  ";
		out << std::setw(10) << std::left << "Name";
		out << std::setw(10) << std::left << "NSpace";
		out << std::setw(10) << std::left << "Author";
		out << std::setw(10) << std::left << "Version";
		out << std::setw(45) << std::left << "Direction";
		out << std::setw(25) << std::left << "Children";
		out << "\n  ";
		out << std::setw(10) << std::left << "--------";
		out << std::setw(10) << std::left << "--------";
		out << std::setw(10) << std::left << "--------";
		out << std::setw(10) << std::left << "--------";
		out << std::setw(40) << std::left << "--------------------------------------";
		out << std::setw(25) << std::left << "-----------------------";
	}

	inline void Payload::showElementPayLoad(NoSqlDb::DbElement<Payload>& elem, std::ostream& out)
	{
		out << "\n  ";
		out << std::setw(10) << std::left << elem.name().substr(0, 8);
		out << std::setw(10) << std::left << elem.payLoad().nameSpace().substr(0, 8);
		out << std::setw(10) << std::left << elem.payLoad().author().substr(0, 8);
		out << std::setw(10) << std::left << elem.payLoad().version();
		out << std::setw(40) << std::left << elem.payLoad().dir().substr(0, 38);
		for (auto cat : elem.payLoad().children())
		{
			out << cat << " ";
		}
	}

	inline void Payload::showDb(NoSqlDb::DbCore<Payload>& db, std::ostream& out)
	{
		showPayLoadHeaders(out);
		for (auto item : db)
		{
			NoSqlDb::DbElement<Payload> temp = item.second;
			Payload::showElementPayLoad(temp, out);
		}
	}
}

#endif