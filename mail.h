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
#include "safe_queue.h"





typedef void (*logprintf_t)(char *format, ...);





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