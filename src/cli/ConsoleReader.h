/*!
 * \brief Helper to read stdin in non-blocking mode.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>
#include <QScopedPointer>

#if defined(Q_OS_WIN)
#include <Winsock2.h>

#include <QThread>
#include <Windows.h>
#else
#include <QSocketNotifier>
#endif


namespace governikus
{

#if defined(Q_OS_WIN)

class ConsoleInputThread
	: public QThread
{
	Q_OBJECT

	public:
		void run() Q_DECL_OVERRIDE;
		virtual ~ConsoleInputThread();

	Q_SIGNALS:
		void fireText(const QString& pData);
};

#endif


class ConsoleReader
	: public QObject
{
	Q_OBJECT

	private:
#if defined(Q_OS_WIN)
		QScopedPointer<ConsoleInputThread> mConsoleInputThread;
#else
		QScopedPointer<QSocketNotifier> mNotifier;

		bool mInputOpen;

	private Q_SLOTS:
		void onData();
#endif

	public:
		ConsoleReader(QObject* pParent = nullptr);
		void init();
		void shutdown();
		bool isInputOpen() const;

		QString readText();

	Q_SIGNALS:
		void fireShutdown();
		void fireText(const QString& pData);
};


} /* namespace governikus */
