#pragma once
#ifndef REPOSITORY_H
#define REPOSITORY_H
#include "../CheckIn/CheckIn.h"
#include "../CheckOut/CheckOut.h"
#include "../Version/Version.h"
#include "../Browse/Browse.h"

namespace repository {
	class RepoCore {
	public:

	private:
		CheckIn ci;
		CheckOut co;
		Version versionManger;
		Browse b;

	};
}


#endif