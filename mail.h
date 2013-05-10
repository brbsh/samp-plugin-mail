#pragma once

#pragma warning(disable:4996)

#include <vector>
#include <iostream>
#include <map>
#include <list>
#include <queue>
#include <memory>

#include "jwsmtp/jwsmtp.h"
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

//Kye's sleep macro
#ifdef WIN32
	#define SLEEP(x) { Sleep(x); }
#else
	#define SLEEP(x) { usleep(x * 1000); }
	typedef unsigned long DWORD;
	typedef unsigned int UINT;
#endif