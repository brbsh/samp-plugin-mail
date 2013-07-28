#pragma once



#include "natives.h"





extern bool gInit;
extern logprintf_t logprintf;
extern amxProcess *gProcess;

extern boost::mutex gMutex;
extern boost::regex gExpression;
//extern boost::atomic<bool> gInit;
extern boost::safe_queue<mailData> amxThreadQueue;





const AMX_NATIVE_INFO amxNatives::mailNatives[] =
{
	{"mail_init", amxNatives::Init},
    {"mail_send", amxNatives::Send},
	{"mail_is_valid", amxNatives::IsValid},

    {NULL, NULL}
};



//native mail_init(host[], user[], password[], from[], sendername[]);
cell AMX_NATIVE_CALL amxNatives::Init(AMX *amx, cell *params)
{
	char *dest = NULL;

	amx_StrParam(amx, params[1], dest);

	if(dest == NULL)
	{
		logprintf("Mail error: NULL host passed to native 'mail_init'");

		return NULL;
	}

	boost::mutex::scoped_lock lock(gMutex);
	gProcess->Config["host"] = dest;
	lock.unlock();

	amx_StrParam(amx, params[2], dest);

	if(dest == NULL)
	{
		logprintf("Mail error: NULL user passed to native 'mail_init'");

		return NULL;
	}

	lock.lock();
	gProcess->Config["user"] = dest;
	lock.unlock();

	amx_StrParam(amx, params[3], dest);

	if(dest == NULL)
	{
		logprintf("Mail error: NULL password passed to native 'mail_init'");

		return NULL;
	}

	lock.lock();
	gProcess->Config["password"] = dest;
	lock.unlock();

	amx_StrParam(amx, params[4], dest);

	if(dest == NULL)
	{
		logprintf("Mail error: NULL from address passed to native 'mail_init'");

		return NULL;
	}

	if(!boost::regex_match(dest, gExpression))
	{
		logprintf("Mail error: Invalid \"from\" email address passed to native 'mail_init'");

		return NULL;
	}

	lock.lock();
	gProcess->Config["from"] = dest;
	lock.unlock();

	amx_StrParam(amx, params[5], dest);

	if(dest == NULL)
	{
		logprintf("Mail error: NULL sender name passed to native 'mail_init'");

		return NULL;
	}

	lock.lock();
	gProcess->Config["sendername"] = dest;
	lock.unlock();
	
	gInit = true;
	
	return 1;
}



// native mail_send(index, to[], subject[], messsage[], type = 0);
cell AMX_NATIVE_CALL amxNatives::Send(AMX *amx, cell *params)
{
	if(!gInit)
	{
		logprintf("Mail error: Can't send any mail. Init phase failed");

		return NULL;
	}

	char *dest = NULL;

	mailData pushme;
	
	pushme.index = params[1];
	amx_StrParam(amx, params[2], dest);

	if(dest == NULL)
	{
		logprintf("Mail error: NULL destination email address passed to native 'mail_send'");

		return NULL;
	}

	if(!boost::regex_match(dest, gExpression))
	{
		logprintf("Mail error: Invalid destination email address passed to native 'mail_send'");

		return NULL;
	}

	pushme.to.assign(dest);
	amx_StrParam(amx, params[3], dest);

	if(dest == NULL)
	{
		logprintf("Mail error: NULL subject passed to native 'mail_send'");

		return NULL;
	}

	pushme.subject.assign(dest);
	amx_StrParam(amx, params[4], dest);

	if(dest == NULL)
	{
		logprintf("Mail error: NULL message passed to native 'mail_send'");

		return NULL;
	}

	pushme.message.assign(dest);
	pushme.type = params[5];

	pushme.error.clear();
	pushme.errorCode = 1;

	amxThreadQueue.push(pushme);

	return 1;
}



// native mail_is_valid(address[]);
cell AMX_NATIVE_CALL amxNatives::IsValid(AMX *amx, cell *params)
{
	char *email = NULL;

	amx_StrParam(amx, params[1], email);

	if(email == NULL)
	{
		logprintf("Mail error: NULL email passed to native 'mail_is_valid'");

		return NULL;
	}

	return (cell)boost::regex_match(email, gExpression);
}
