#pragma once
#pragma warning(disable:4996)

#define _CRT_SECURE_NO_WARNINGS



#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <queue>

//#include <boost/atomic.hpp>
#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/thread.hpp>
#include <boost/unordered_map.hpp>

#define HAVE_STDINT_H

#include "SDK/plugin.h"
#include "jwsmtp/jwsmtp.h"

#include "natives.h"
#include "process.h"





typedef void (*logprintf_t)(char *format, ...);


#define PLUGIN_VERSION "1.4"

#define arguments(a) \
	!(params[0] != (a << 2))





struct mailData
{
	int index;
	int type;
	int errorCode;

	std::string to;
	std::string subject;
	std::string message;
	std::string error;
};