/*
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "SignalHandler.h"

#ifdef Q_OS_WIN
	#include "BuildHelper.h"
#endif

#include <QCoreApplication>
#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(system)


SignalHandler::SignalHandler()
	: QObject()
	, mInit(false)
	, mController()
#ifdef Q_OS_WIN
	, mServiceStatusHandle(nullptr)
	, mLock()
#endif
{
}


#if defined(Q_OS_WIN) && !defined(Q_OS_WINRT)
SignalHandler::~SignalHandler()
{
	SetConsoleCtrlHandler(nullptr, false);
	setServiceStatus(SERVICE_STOPPED);
}


#else
SignalHandler::~SignalHandler() = default;


#endif


void SignalHandler::init()
{
	if (!mInit)
	{
#if (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)) || (defined(Q_OS_BSD4) && !defined(Q_OS_IOS))
		initUnix();
#elif defined(Q_OS_WIN) && !defined(Q_OS_WINRT)
		SetConsoleCtrlHandler(PHANDLER_ROUTINE(ctrlHandler), true);
		if (!BuildHelper::isUserInteractive())
		{
			registerService();
		}

#endif

		mInit = true;
	}
}


void SignalHandler::setController(const std::function<void()>& pController)
{
	mController = pController;
}


void SignalHandler::quit()
{
#ifdef Q_OS_WIN
	setServiceStatus(SERVICE_STOP_PENDING);
#endif

	if (mController)
	{
		mController();
	}
	else
	{
		QCoreApplication::exit();
	}
}


#if !(defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS))
// A dummy is required for all platform since moc fails to handle some defines.
void SignalHandler::onSignalSocketActivated()
{
}


#endif
