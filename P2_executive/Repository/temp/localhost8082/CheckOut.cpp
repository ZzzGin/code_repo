// CheckOut.cpp : Defines the entry point for the console application.
//
#include "CheckOut.h"

using namespace repository;
using namespace NoSqlDb;

CheckOut::CheckOut(DbCore<Payload> &db, CheckOutInfo &coi) 
	: db_(db)
	, coi_(coi)
	, versionManager_(db, coi_.fileName(), coi_.nameSpace()) {

}

void clone(std::string dst) {

}


int main()
{
    return 0;
}

