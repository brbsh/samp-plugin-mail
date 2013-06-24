#pragma once
#pragma warning(disable:4996)



#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <list>
#include <queue>

#include <boost/regex.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>

#define HAVE_STDINT_H

#include "SDK/plugin.h"
#include "jwsmtp/jwsmtp.h"

#include "natives.h"
#include "process.h"





//Kye's sleep macro
#ifdef WIN32
	#define SLEEP(x) { Sleep(x); }
#else
	#define SLEEP(x) { usleep(x * 1000); }
	typedef unsigned long DWORD;
	typedef unsigned int UINT;
#endif





typedef void (*logprintf_t)(char *format, ...);





struct mailData
{
	int index;
	std::string to;
	std::string subject;
	std::string message;
	int type;
	std::string error;
	int errorCode;
};