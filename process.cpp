#pragma once



#include "process.h"




amxProcess *gProcess;


extern logprintf_t logprintf;

extern std::queue<mailData> amxThreadQueue;
extern std::queue<mailData> amxProcessTickQueue;





amxProcess::amxProcess()
{
	this->Active = true;

	boost::thread mail(boost::bind(&amxProcess::Thread));
}



amxProcess::~amxProcess()
{
	this->Active = false;
}



void amxProcess::Thread()
{
	boost::mutex localMutex;

	do
	{
		while(!amxThreadQueue.empty())
		{
			mailData data;

			jwsmtp::mailer mail;

			boost::mutex::scoped_lock lock(localMutex);
			data = amxThreadQueue.front();
			amxThreadQueue.pop();
			lock.unlock();

			mail.addrecipient(data.to);
			mail.setsubject(data.subject);
			mail.setmessage(data.message);

			mail.setsender(gProcess->Config.find("from")->second);
			mail.setsendername(gProcess->Config.find("sendername")->second);
			mail.setserver(gProcess->Config.find("host")->second);
			mail.username(gProcess->Config.find("user")->second);
			mail.password(gProcess->Config.find("password")->second);

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

		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}
	while(gProcess->Active);
}