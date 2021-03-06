#include "Payload.h"
#include "../NoSqlDb/Utilities/StringUtilities/StringUtilities.h"
#include "../NoSqlDb/Persist/Persist.h"

using namespace NoSqlDb;
using namespace XmlProcessing;

#ifdef TEST_PAYLOAD

int main() {
	DbCore<Payload> db;
	DbElement<Payload> demoElem;

	demoElem.name("Jim");
	demoElem.descrip("Instructor for CSE687");
	demoElem.dateTime(DateTime().now());

	Payload pl;
	pl.dir("c://demo");
	pl.children().push_back("1");
	pl.children().push_back("2");
	pl.children().push_back("3");
	pl.version(0);
	pl.status(false);
	pl.author("qj");
	pl.nameSpace("NoSqlDb");
	demoElem.payLoad(pl);
	db["Fawcett"] = demoElem;
	showDb(db);
	Utilities::putline();

	pl.showDb(db);

	Utilities::putline();
	Persist<Payload> persist(db);

	std::string xml = persist.toXml();
	std::cout << xml;

	DbCore<Payload> newdb;
	Persist<Payload> newpersist(newdb);
	newpersist.fromXml(xml, rebuild);

	showDb(newdb);
	pl.showDb(newdb);
	Utilities::putline();
	return 0;
}

#endif