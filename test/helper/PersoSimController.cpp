/*!
 * PersoSimController.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "PersoSimController.h"

#include <signal.h>

#include <QEventLoop>

#include "testMacros.h"


using governikus::PersoSimController;


static const char* const sPersoSimPrompt = "PersoSim commandline: ";

PersoSimController* PersoSimController::sActiveController = nullptr;
bool PersoSimController::sAbortSignalHandlerInstalled = false;


// We very much want to clean up the active PersoSimController -- since
// otherwise the PersoSim process keeps running -- so we use a global
// destructor to destroy it.
struct CleanupHelper
{
	~CleanupHelper()
	{
		PersoSimController::cleanUpActiveController();
	}


};

static CleanupHelper sCleanupHelper;


PersoSimController::PersoSimController()
	: QObject()
	, mProcess(nullptr)
	, mEventLoop(nullptr)
{
}


PersoSimController::~PersoSimController()
{
	if (sActiveController == this)
	{
		sActiveController = nullptr;
	}

	if (mProcess != nullptr)
	{
		QCOMPARE(mProcess->write("exit\n"), 5);
		mProcess->waitForFinished(-1);
	}
}


bool PersoSimController::init()
{
#ifdef PERSOSIM_TESTS_ENABLED
	return startProcess();

#else
	XSKIP("PersoSim tests not enabled", false);
#endif
}


void PersoSimController::cleanUpActiveController()
{
	delete sActiveController;
	sActiveController = nullptr;
}


bool PersoSimController::startProcess()
{
	XVERIFY(mProcess == nullptr, false);
	XVERIFY(sActiveController == nullptr, false);

	mProcess = new QProcess;
	mProcess->setProgram(JAVA_EXECUTABLE);
	mProcess->setArguments(QStringList() << "-jar" << PERSOSIM_EXECUTABLE << "--consoleOnly");
	mProcess->setProcessChannelMode(QProcess::ForwardedErrorChannel);

	connect(mProcess, &QProcess::readyRead, this, &PersoSimController::inputAvailable);
	connect(mProcess, QOverload<int>::of(&QProcess::finished), this, &PersoSimController::processFinished);

	mProcess->start();
	XVERIFY(mProcess->waitForStarted(-1), false);
	XCOMPARE(mProcess->state(), QProcess::Running, false);

	sActiveController = this;
	installAbortSignalHandler();

	// wait for PersoSim command line prompt
	QEventLoop eventLoop;
	mEventLoop = &eventLoop;

	int result = eventLoop.exec();
	mEventLoop = nullptr;
	XCOMPARE(result, 0, false);

	return true;
}


void PersoSimController::inputAvailable()
{

	QByteArray line = mProcess->readLine();
	if (line.isEmpty())
	{
		return;
	}

	if (line.endsWith('\n'))
	{
		line.truncate(line.length() - 1);
	}

	if (line == sPersoSimPrompt)
	{
		if (mEventLoop != nullptr)
		{
			mEventLoop->exit(0);
		}
	}

	qDebug("PersoSim: %s", line.data());
}


void PersoSimController::processFinished(int /*pExitCode*/)
{
	if (mEventLoop != nullptr)
	{
		mEventLoop->exit(1);
	}

	if (sActiveController == this)
	{
		sActiveController = nullptr;
	}
}


void PersoSimController::installAbortSignalHandler()
{
	// We very much want to clean up the active PersoSimController -- since
	// otherwise the PersoSim process keeps running -- so we install a SIGABRT
	// signal handler for the case that something misbehaves (triggering an
	// assert or throwing an uncaught exception) and abort() is called.

	if (!sAbortSignalHandlerInstalled)
	{
		signal(SIGABRT, &abortSignalHandler);
		sAbortSignalHandlerInstalled = true;
	}
}


void PersoSimController::abortSignalHandler(int /*pSignal*/)
{
	cleanUpActiveController();
}
