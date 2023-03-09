/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Integration tests for integrated SDK.
 */

#include "AusweisApp2_p.h"

#include "QtHooks.h"

#include <cstring>
#include <iostream>
#include <thread>

static int cExitCode = -1;

void cb(const char* pMessage)
{
	std::cout << "\x1b[1m";
	std::cout << "**** Callback thread id: " << std::this_thread::get_id() << std::endl;

	if (pMessage == nullptr)
	{
		std::cout << "**** AusweisApp2 is initialized" << "\x1b[0m" << std::endl;
		ausweisapp2_send(R"({"cmd": "GET_INFO"})");
		ausweisapp2_send(R"({"cmd": "RUN_AUTH", "tcTokenURL": "https://test.governikus-eid.de/AusweisAuskunft/WebServiceRequesterServlet?mode=json"})");
		return;
	}

	const std::string s(pMessage);
	std::cout << "**** Callback msg: " << s << std::endl;

	if (s.find(R"("msg":"ACCESS_RIGHTS")") != std::string::npos)
	{
		ausweisapp2_send(R"({"cmd": "ACCEPT"})");
	}

	if (s.find(R"("msg":"INSERT_CARD")") != std::string::npos || s.find(R"("msg":"ENTER_)") != std::string::npos)
	{
		ausweisapp2_send(R"({"cmd": "CANCEL"})");
	}

	if (s.find(R"("msg":"AUTH")") != std::string::npos && s.find(R"(sal#cancellationByUser)") != std::string::npos)
	{
		std::cout << "**** Finished" << std::endl;
		ausweisapp2_shutdown();
		cExitCode = 0;
	}

	std::cout << "\x1b[0m" << std::endl;
}


void start_aa2(const char* pParameter)
{
	ausweisapp2_init(&cb, pParameter);

	std::cout << "Let's wait here..." << std::endl;
	governikus::ausweisapp2_join_thread_internal();

	if (ausweisapp2_is_running())
	{
		ausweisapp2_shutdown();
	}
}


int main()
{
	governikus::QtHooks::init();

	start_aa2(nullptr);

	const int livingDeadCount = static_cast<int>(governikus::QtHooks::getQObjects().size());
	if (livingDeadCount > 0)
	{
		governikus::QtHooks::setPrint();
		governikus::QtHooks::printAlive();
		std::cout << "There are zombies behind you: " << livingDeadCount << std::endl;

#ifdef GOVERNIKUS_QT
		return livingDeadCount;

#endif
	}

	if (cExitCode == 0)
	{
		cExitCode = -1;
		start_aa2("--no-loghandler");
	}

	return cExitCode;
}
