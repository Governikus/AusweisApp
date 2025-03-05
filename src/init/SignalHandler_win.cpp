/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "SignalHandler.h"

#include "Env.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QReadLocker>
#include <QThreadPool>
#include <QWriteLocker>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(system)

#define SERVICE_NAME TEXT("AusweisApp")

namespace
{
const char* toPrintable(DWORD pState)
{
	switch (pState)
	{
		case SERVICE_CONTINUE_PENDING:
			return "SERVICE_CONTINUE_PENDING";

		case SERVICE_PAUSE_PENDING:
			return "SERVICE_PAUSE_PENDING";

		case SERVICE_PAUSED:
			return "SERVICE_PAUSED";

		case SERVICE_RUNNING:
			return "SERVICE_RUNNING";

		case SERVICE_START_PENDING:
			return "SERVICE_START_PENDING";

		case SERVICE_STOP_PENDING:
			return "SERVICE_STOP_PENDING";

		case SERVICE_STOPPED:
			return "SERVICE_STOPPED";

		default:
			return "UNKNOWN";
	}

	Q_UNREACHABLE();
}


} // namespace


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


VOID WINAPI SignalHandler::serviceCtrlHandler(DWORD pCtrlCode)
{
	switch (pCtrlCode)
	{
		case SERVICE_CONTROL_STOP:
		{
			QMetaObject::invokeMethod(Env::getSingleton<SignalHandler>(),
					&SignalHandler::quit);
			break;
		}

		default:
			break;
	}
}


void SignalHandler::registerService()
{
	QThreadPool::globalInstance()->start([]{
				QThread::currentThread()->setObjectName(QStringLiteral("ServiceDispatcherThread"));

				SERVICE_TABLE_ENTRY ServiceTable[] =
				{
					{const_cast<LPTSTR>(SERVICE_NAME), static_cast<LPSERVICE_MAIN_FUNCTION>(serviceMain)},
					{nullptr, nullptr}
				};

				qCDebug(system) << "Start ServiceCtrlDispatcher";
				if (!StartServiceCtrlDispatcher(ServiceTable))
				{
					qCCritical(system) << "Cannot start ServiceCtrlDispatcher:" << GetLastError();
				}
				qCDebug(system) << "ServiceCtrlDispatcher returned...";
			});
}


VOID WINAPI SignalHandler::serviceMain(DWORD pArgc, LPTSTR* pArgv)
{
	Q_UNUSED(pArgc)
	Q_UNUSED(pArgv)

	qCDebug(system) << "Register ServiceCtrlHandler";
	auto* handler = Env::getSingleton<SignalHandler>();
	{
		const QWriteLocker locker(&handler->mLock);

		handler->mServiceStatusHandle = RegisterServiceCtrlHandler(const_cast<LPTSTR>(SERVICE_NAME), serviceCtrlHandler);
		if (!handler->mServiceStatusHandle)
		{
			qCCritical(system) << "Cannot register ServiceCtrlHandler:" << GetLastError();
			return;
		}
	}

	handler->setServiceStatus(SERVICE_START_PENDING);

	QMetaObject::invokeMethod(handler, [handler]{
				handler->setServiceStatus(SERVICE_RUNNING);
			});
}


void SignalHandler::setServiceStatus(DWORD pCurrentState)
{
	static DWORD dwCheckPoint = 0;

	const QReadLocker locker(&mLock);
	if (mServiceStatusHandle)
	{
		SERVICE_STATUS status;
		status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		status.dwServiceSpecificExitCode = 0;
		status.dwWin32ExitCode = 0;
		status.dwCurrentState = pCurrentState;

		if (pCurrentState == SERVICE_START_PENDING)
		{
			status.dwControlsAccepted = 0;
		}
		else
		{
			status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		}

		if ((pCurrentState == SERVICE_RUNNING) || (pCurrentState == SERVICE_STOPPED))
		{
			status.dwCheckPoint = 0;
		}
		else
		{
			status.dwCheckPoint = dwCheckPoint++;
		}

		qCDebug(system) << "Set service status:" << toPrintable(pCurrentState);
		if (!SetServiceStatus(mServiceStatusHandle, &status))
		{
			qCCritical(system) << "Cannot set service status:" << GetLastError();
		}
	}
}
