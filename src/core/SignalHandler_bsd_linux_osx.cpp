/*
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "SignalHandler.h"

#include "SingletonHelper.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <sys/socket.h>
#include <unistd.h>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(system)

int SignalHandler::cSignalSocketPair[2];


void SignalHandler::initUnix()
{
	// Signal handling on POSIX systems is some what messed up since there exists
	// only a limited set of API calls which are usable. Therefore we create a private
	// socketpair to cross thread borders.
	// http://doc.qt.io/qt-5.7/unix-signals.html

	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, cSignalSocketPair))
	{
		qCritical(system) << "** Failed to set up socket pair for signaling! **";
		return;
	}

	mSignalSocketNotifier.reset(new QSocketNotifier(cSignalSocketPair[1], QSocketNotifier::Read));
	mSignalSocketNotifier->setEnabled(true);
	connect(mSignalSocketNotifier.data(), &QSocketNotifier::activated, this, &SignalHandler::onSignalSocketActivated);

	::signal(SIGINT, sigHandler);
	::signal(SIGTERM, sigHandler);
	::signal(SIGHUP, sigHandler);
	::signal(SIGQUIT, sigHandler);
}


void SignalHandler::sigHandler(int pSignal)
{
	::write(cSignalSocketPair[0], &pSignal, sizeof(pSignal));
}


void SignalHandler::onSignalSocketActivated()
{
	mSignalSocketNotifier->setEnabled(false);

	int signal;
	::read(cSignalSocketPair[1], &signal, sizeof(signal));

	qCWarning(system) << "Got signal:" << signal;

	switch (signal)
	{
		case SIGINT:
		case SIGTERM:
		case SIGHUP:
		case SIGQUIT:
			getInstance().quit();
			break;

		default:
			break;
	}

	mSignalSocketNotifier->setEnabled(true);
}
