/*
 * \brief Implements signal handler for unix and windows.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "controller/AppController.h"

#include <QPointer>

#if defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
#include <csignal>
#include <QSocketNotifier>
#elif defined(Q_OS_WIN)
#include <windows.h>
#endif

namespace governikus
{

class SignalHandler
	: public QObject
{
	Q_OBJECT

	private:
		Q_DISABLE_COPY(SignalHandler)

		bool mInit;
		QPointer<AppController> mAppController;
		bool mQuit;

		void quit();

#if defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)

	private:
		QScopedPointer<QSocketNotifier> mSignalSocketNotifier;
		static int cSignalSocketPair[2];

		void initUnix();
		static void sigHandler(int pSignal);
#elif defined(Q_OS_WIN)

	private:
		static BOOL WINAPI ctrlHandler(DWORD pCtrlType);
#endif

	private Q_SLOTS:
		void onSignalSocketActivated();

	protected:
		SignalHandler();
		virtual ~SignalHandler();

	public:
		static SignalHandler& getInstance();
		void init();
		void setController(AppController& pAppController);
		bool shouldQuit() const;
};

} // namespace governikus
