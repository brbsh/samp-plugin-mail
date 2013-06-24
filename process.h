#pragma once



#include "mail.h"





class amxProcess
{

public:

	amxProcess();
	~amxProcess();

	static void Thread();

	bool Active;

	boost::mutex Mutex;
	boost::unordered_map<std::string, std::string> Config;
};