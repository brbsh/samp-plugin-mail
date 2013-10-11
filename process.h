#pragma once



#include "mail.h"





class amxProcess
{

public:

	bool Active;

	boost::unordered_map<std::string, std::string> Config;

	amxProcess();
	~amxProcess();

	static void Thread();
};