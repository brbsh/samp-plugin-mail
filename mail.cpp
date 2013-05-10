#include "mail.h"

#ifdef WIN32
	HANDLE threadHandle;
	HANDLE mutexHandle;
	DWORD __stdcall MailThread(LPVOID lpParam);
#else
	pthread_mutex_t mutexHandle;
	void * MailThread(void *lpParam);
#endif



typedef void (*logprintf_t)(char* format, ...);
logprintf_t logprintf;
extern void *pAMXFunctions;

void lockMutex();
void unlockMutex();



struct mail_struct
{
	std::string to;
	std::string subject;
	std::string message;
	short type;
	int errorCode;
};



std::list<AMX*> mail_amx_list;
std::queue<mail_struct> mail_thread_queue;
std::queue<mail_struct> mail_pt_queue;
std::map<std::string, std::string> mail_config;





PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) 
{
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];
	
    logprintf("  Mail plugin loaded");

	#ifdef WIN32
		DWORD dwThreadId = 0;
		threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MailThread, NULL, 0, &dwThreadId);
		CloseHandle(threadHandle);
		mutexHandle = CreateMutex(NULL, FALSE, LPCSTR("pawnmail"));
	#else
		pthread_t threadHandle;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&threadHandle, &attr, &MailThread, NULL);
		mutexHandle = PTHREAD_MUTEX_INITIALIZER;
	#endif

    return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	#ifdef WIN32
		CloseHandle(mutexHandle);
	#else
		pthread_mutex_destroy(&mutexHandle);
	#endif

	mail_amx_list.clear();

	logprintf("  Mail plugin unloaded");
}

#ifdef WIN32
	DWORD __stdcall MailThread(LPVOID lpParam)
#else
	void * MailThread(void *lpParam)
#endif
{
	mail_struct mail_thread;
	jwsmtp::mailer mail;

	while(true)
	{
		if(!mail_thread_queue.empty())
		{
			for(unsigned int i = 0; i < mail_thread_queue.size(); i++)
			{
				lockMutex();
				mail_thread = mail_thread_queue.front();
				mail_thread_queue.pop();
				unlockMutex();

				mail.addrecipient(mail_thread.to);
				mail.setsubject(mail_thread.subject);
				mail.setmessage(mail_thread.message);

				lockMutex();

				mail.setsender(mail_config.find("from")->second);
				mail.setsendername(mail_config.find("sendername")->second);

				mail.setserver(mail_config.find("host")->second);
				mail.username(mail_config.find("user")->second);
				mail.password(mail_config.find("password")->second);

				unlockMutex();

				if(mail_thread.type)
				{
					mail.setmessage(" ");
					mail.setmessageHTML(mail_thread.message);
				}

				mail.send();
				mail_thread.errorCode = atoi(mail.response().c_str());

				mail_pt_queue.push(mail_thread);
			}
		}

		SLEEP(1);
	}
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	mail_struct mail_pt;

	cell amx_Address[3], *phys_addr; 
	int amx_Idx;

	if(!mail_pt_queue.empty())
	{
		for(unsigned int i = 0; i < mail_pt_queue.size(); i++)
		{
			lockMutex();
			mail_pt = mail_pt_queue.front();
			mail_pt_queue.pop();
			unlockMutex();

			if(mail_pt.errorCode == 250)
			{
				for(std::list<AMX*>::iterator amx = mail_amx_list.begin(); amx != mail_amx_list.end(); amx++)
				{
					if(!amx_FindPublic(*amx, "OnMailSendSuccess", &amx_Idx))
					{
						//forward OnMailSendSuccess(to[], subject[], message[], type);
						amx_Push(*amx, mail_pt.type);
						amx_PushString(*amx, &amx_Address[0], &phys_addr, mail_pt.message.c_str(), 0, 0);
						amx_PushString(*amx, &amx_Address[1], &phys_addr, mail_pt.subject.c_str(), 0, 0);
						amx_PushString(*amx, &amx_Address[2], &phys_addr, mail_pt.to.c_str(), 0, 0);

						amx_Exec(*amx, NULL, amx_Idx);

						amx_Release(*amx, amx_Address[0]);
						amx_Release(*amx, amx_Address[1]);
						amx_Release(*amx, amx_Address[2]);
					}
				}
			}
			else
			{
				for(std::list<AMX*>::iterator amx = mail_amx_list.begin(); amx != mail_amx_list.end(); amx++)
				{
					if(!amx_FindPublic(*amx, "OnMailSendError", &amx_Idx))
					{
						//forward OnMailSendError(to[], subject[], message[], type, error_code);
						amx_Push(*amx, mail_pt.errorCode);
						amx_Push(*amx, mail_pt.type);
						amx_PushString(*amx, &amx_Address[0], &phys_addr, mail_pt.message.c_str(), 0, 0);
						amx_PushString(*amx, &amx_Address[1], &phys_addr, mail_pt.subject.c_str(), 0, 0);
						amx_PushString(*amx, &amx_Address[2], &phys_addr, mail_pt.to.c_str(), 0, 0);

						amx_Exec(*amx, NULL, amx_Idx);

						amx_Release(*amx, amx_Address[0]);
						amx_Release(*amx, amx_Address[1]);
						amx_Release(*amx, amx_Address[2]);
					}
				}
			}
		}
	}
}

//native mail_init(host[], user[], password[], from[], sendername[]);
cell AMX_NATIVE_CALL n_mail_init(AMX *amx, cell *params)
{
	char *host;
	char *user;
	char *password;
	char *from;
	char *sendername;
	std::string temp;

	amx_StrParam(amx, params[1], host);
	amx_StrParam(amx, params[2], user);
	amx_StrParam(amx, params[3], password);
	amx_StrParam(amx, params[4], from);
	amx_StrParam(amx, params[5], sendername);

	lockMutex();

	temp.assign(host);
	mail_config["host"] = temp;

	temp.assign(user);
	mail_config["user"] = temp;

	temp.assign(password);
	mail_config["password"] = temp;

	temp.assign(from);
	mail_config["from"] = temp;

	temp.assign(sendername);
	mail_config["sendername"] = temp;

	unlockMutex();

	return 1;
}

// native mail_send(to[], subject[], messsage[], type = 0);
cell AMX_NATIVE_CALL n_mail_send(AMX *amx, cell *params)
{
	mail_struct push_to_thread;
	char *to;
	char *subject;
	char *message;
	
	amx_StrParam(amx, params[1], to);
	amx_StrParam(amx, params[2], subject);
	amx_StrParam(amx, params[3], message);

	push_to_thread.to.assign(to);
	push_to_thread.subject.assign(subject);
	push_to_thread.message.assign(message);
	push_to_thread.type = static_cast<short>(params[4]);

	delete to;
	delete subject;
	delete message;

	lockMutex();
	mail_thread_queue.push(push_to_thread);
	unlockMutex();

	return 1;
}

AMX_NATIVE_INFO PluginNatives[] =
{
	{"mail_init", n_mail_init},
    {"mail_send", n_mail_send},
    {0, 0}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) 
{
	mail_amx_list.push_back(amx);

    return amx_Register(amx, PluginNatives, -1);
}


PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) 
{
	for(std::list<AMX*>::iterator i = mail_amx_list.begin(); i != mail_amx_list.end(); i++) 
	{
		if(*i == amx) 
		{
			mail_amx_list.erase(i);

			break;
		}
	}

    return AMX_ERR_NONE;
} 

void lockMutex()
{
	#ifdef WIN32
		WaitForSingleObject(mutexHandle, INFINITE);
	#else
		pthread_mutex_lock(&mutexHandle);
	#endif
}

void unlockMutex()
{
	#ifdef WIN32
		ReleaseMutex(mutexHandle);
	#else
		pthread_mutex_unlock(&mutexHandle);
	#endif
}