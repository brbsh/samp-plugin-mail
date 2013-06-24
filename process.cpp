#pragma once



#include "process.h"




amxProcess *gProcess;
extern logprintf_t logprintf;

extern std::queue<mailData> amxThreadQueue;
extern std::queue<mailData> amxProcessTickQueue;





amxProcess::amxProcess()
{
	this->Active = true;
}



amxProcess::~amxProcess()
{
	this->Active = false;
}



void amxProcess::Thread()
{
	do
	{
		if(!amxThreadQueue.empty())
		{
			jwsmtp::mailer mail;
			mailData data;

			for(unsigned int i = 0; i < amxThreadQueue.size(); i++)
			{
				boost::mutex::scoped_lock lock(gProcess->Mutex);
				data = amxThreadQueue.front();
				amxThreadQueue.pop();
				lock.unlock();

				mail.addrecipient(data.to);
				mail.setsubject(data.subject);
				mail.setmessage(data.message);

				mail.setsender(gProcess->Config["from"]);
				mail.setsendername(gProcess->Config["sendername"]);

				mail.setserver(gProcess->Config["host"]);
				mail.username(gProcess->Config["user"]);
				mail.password(gProcess->Config["password"]);

				if(data.type == 1)
				{
					mail.setmessage(" ");
					mail.setmessageHTML(data.message);
				}

				mail.send();

				data.error.assign(mail.response());
				data.errorCode = atoi(data.error.c_str());

				lock.lock();
				amxProcessTickQueue.push(data);
				lock.unlock();
			}
		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}
	while(gProcess->Active);
}