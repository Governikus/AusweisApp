/*
 * \brief Implements signal handler for unix and windows.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>
#include <functional>

#if defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	#include <QSocketNotifier>
	#include <csignal>
#elif defined(Q_OS_WIN)
	#include <windows.h>
#endif

namespace governikus
{

class SignalHandler
	: public QObject
{
	Q_OBJECT

	Q_DISABLE_COPY(SignalHandler)
	friend class Env;

	private:
		bool mInit;
		std::function<void()> mController;
		bool mQuit;

		SignalHandler();
		~SignalHandler() override;
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

	public:
		void init();
		void setController(const std::function<void()>& pController);
		[[nodiscard]] bool shouldQuit() const;
};

} // namespace governikus
