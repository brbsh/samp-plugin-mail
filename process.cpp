#pragma once



#include "process.h"




amxProcess *gProcess;


extern logprintf_t logprintf;

extern boost::safe_queue<mailData> amxThreadQueue;
extern boost::safe_queue<mailData> amxProcessTickQueue;





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
	boost::mutex tMutex;

	do
	{
		if(!amxThreadQueue.empty())
		{
			mailData data;

			jwsmtp::mailer mail;

			data = amxThreadQueue.pop();

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

			amxProcessTickQueue.push(data);
		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}
	while(gProcess->Active);
}