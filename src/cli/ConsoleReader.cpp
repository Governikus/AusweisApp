/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "ConsoleReader.h"

#include <QEventLoop>
#include <QLoggingCategory>
#include <QTimer>

Q_DECLARE_LOGGING_CATEGORY(cli)
Q_DECLARE_LOGGING_CATEGORY(stdinput)

using namespace governikus;


#if defined(Q_OS_WIN)

void ConsoleInputThread::run()
{
	const HANDLE eventHandles = GetStdHandle(STD_INPUT_HANDLE);
	while (!isInterruptionRequested())
	{
		if (WSA_WAIT_EVENT_0 != WSAWaitForMultipleEvents(1, &eventHandles, FALSE, 200, TRUE))
		{
			continue;
		}

		QTextStream input(stdin, QIODevice::ReadOnly);
		const auto& line = input.readLine();
		qDebug(stdinput) << line;
		Q_EMIT fireText(line);
	}
}


ConsoleInputThread::~ConsoleInputThread()
{
	requestInterruption();
	wait();
}


ConsoleReader::ConsoleReader(QObject* pParent)
	: QObject(pParent)
{
}


void ConsoleReader::init()
{
	mConsoleInputThread.reset(new ConsoleInputThread());
	connect(mConsoleInputThread.data(), &ConsoleInputThread::fireText, this, &ConsoleReader::fireText);
	mConsoleInputThread->start();
}


void ConsoleReader::shutdown()
{
	mConsoleInputThread.reset();
}


bool ConsoleReader::isInputOpen() const
{
	return !mConsoleInputThread.isNull();
}


#else


ConsoleReader::ConsoleReader(QObject* pParent)
	: QObject(pParent)
	, mNotifier()
	, mInputOpen(false)
{
}


void ConsoleReader::init()
{
	Q_ASSERT(mNotifier.isNull());
	mNotifier.reset(new QSocketNotifier(fileno(stdin), QSocketNotifier::Read));
	connect(mNotifier.data(), &QSocketNotifier::activated, this, &ConsoleReader::onData);
	mInputOpen = true;
}


void ConsoleReader::onData()
{
	Q_EMIT fireText(readText());
}


QString ConsoleReader::readText()
{
	Q_ASSERT(!mNotifier.isNull());

	QTextStream input(stdin, QIODevice::ReadOnly);
	const QString& line = input.readLine();
	if (input.status() == QTextStream::ReadPastEnd)
	{
		mInputOpen = false;
	}
	return line;
}


void ConsoleReader::shutdown()
{
	Q_ASSERT(!mNotifier.isNull());
	mInputOpen = false;
	mNotifier->setEnabled(false);

	Q_EMIT fireShutdown();
}


bool ConsoleReader::isInputOpen() const
{
	return mInputOpen;
}


#endif
