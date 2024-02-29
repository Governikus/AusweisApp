/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*
 * \brief Implements signal handler for unix and windows.
 */

#pragma once

#include "Env.h"

#include <QObject>
#include <functional>

#if defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	#include <QSocketNotifier>
	#include <csignal>
#elif defined(Q_OS_WIN)
	#include <QReadWriteLock>
	#include <windows.h>
#endif

namespace governikus
{

class SignalHandler
	: public QObject
	, private Env::ThreadSafe
{
	Q_OBJECT

	Q_DISABLE_COPY(SignalHandler)
	friend class Env;
	friend void shutdownApp();

	private:
		bool mInit;
		std::function<void()> mController;

#ifdef Q_OS_WIN
		SERVICE_STATUS_HANDLE mServiceStatusHandle;
		QReadWriteLock mLock;
#endif

		SignalHandler();
		~SignalHandler() override;

#if defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)

	private:
		QScopedPointer<QSocketNotifier> mSignalSocketNotifier;
		static int cSignalSocketPair[2];

		void initUnix();
		static void sigHandler(int pSignal);
#elif defined(Q_OS_WIN)

	private:
		static BOOL WINAPI ctrlHandler(DWORD pCtrlType);
		static VOID WINAPI serviceMain(DWORD pArgc, LPTSTR* pArgv);
		static VOID WINAPI serviceCtrlHandler(DWORD pCtrlCode);
		static void registerService();
		void setServiceStatus(DWORD pCurrentState);
#endif

	private Q_SLOTS:
		void onSignalSocketActivated();
#if defined(Q_OS_WIN)
		void quit();
#else
		void quit() const;
#endif

	public:
		void init();
		void setController(const std::function<void()>& pController);
};

} // namespace governikus
