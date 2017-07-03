/*!
 * PersoSimController.h
 *
 * \brief Controller for an external PersoSim process.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QByteArray>
#include <QObject>
#include <QProcess>
#include <QtTest>

#if defined(PERSOSIM_EXECUTABLE) && defined(JAVA_EXECUTABLE)
#   define PERSOSIM_TESTS_ENABLED
#   define SKIP_IF_PERSOSIM_DISABLED() do {}\
	while (false)
#else
#   define PERSOSIM_EXECUTABLE ""
#   define JAVA_EXECUTABLE ""
#   define SKIP_IF_PERSOSIM_DISABLED() QSKIP("PersoSim tests not enabled", false);
#endif

class QEventLoop;

namespace governikus
{

class PersoSimController
	: public QObject
{
	Q_OBJECT

	public:
		PersoSimController();
		~PersoSimController();

		bool init();

		static void cleanUpActiveController();

	private:
		bool startProcess();

		void inputAvailable();
		void processFinished(int pExitCode);

		static void installAbortSignalHandler();
		static void abortSignalHandler(int pSignal);

	private:
		QProcess* mProcess;
		QEventLoop* mEventLoop;

		static PersoSimController* sActiveController;
		static bool sAbortSignalHandlerInstalled;
};

} /* namespace governikus */
