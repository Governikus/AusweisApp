/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "SignalHandler.h"

#include "Env.h"

#include <QCoreApplication>
#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(system)


BOOL WINAPI SignalHandler::ctrlHandler(DWORD pCtrlType)
{
	qCWarning(system) << "Got signal:" << pCtrlType;

	switch (pCtrlType)
	{
		case CTRL_C_EVENT:
			Env::getSingleton<SignalHandler>()->quit();
			return true;

		default:
			return false;
	}
}
