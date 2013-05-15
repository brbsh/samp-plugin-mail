#pragma once

#pragma warning(disable:4996)

#include <stdlib.h>
#include <vector>
#include <iostream>
#include <map>
#include <list>
#include <queue>
#include <memory>

#include "sdk/amx/amx.h"
#include "sdk/plugincommon.h"
#include "jwsmtp/jwsmtp.h"

//Kye's sleep macro
#ifdef WIN32
	#define SLEEP(x) { Sleep(x); }
#else
	#define SLEEP(x) { usleep(x * 1000); }
	typedef unsigned long DWORD;
	typedef unsigned int UINT;
#endif