/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "CliHelper.h"

#include <QCoreApplication>

#include <cstdint>

using namespace governikus;

CliHelper::CliHelper(QObject* pParent)
	: QProcess(pParent)
	, mTimer()
	, mLoop()
	, mOutput()
	, mServerPort(0)
{
	qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");

	mTimer.setSingleShot(true);
	connect(&mTimer, &QTimer::timeout, &mLoop, &QEventLoop::quit);
	connect(this, &QProcess::readyRead, this, &CliHelper::storeOutput);

	QString path = QCoreApplication::applicationDirPath() + "/../../src/";
	QString app = path + "AusweisApp2";
	QStringList args;
	args << "--ui" << "cli";
	args << "--port" << "0";

#ifdef Q_OS_WIN
	app += ".exe";
#else
	args << "-platform" << "offscreen";
#endif

	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	env.insert("QT_LOGGING_TO_CONSOLE", "1");
	setProcessEnvironment(env);
	setProgram(app);
	setArguments(args);
	setWorkingDirectory(path);
	setReadChannel(QProcess::StandardError);
}


CliHelper::~CliHelper()
{
	disconnect(this, &QProcess::readyRead, this, &CliHelper::storeOutput);
	tearDown();
}


void CliHelper::storeOutput()
{
	while (canReadLine())
	{
		mOutput += QString::fromUtf8(readLine());
	}

	if (!mOutput.isEmpty())
	{
		mLoop.quit();
	}
}


void CliHelper::waitForOutput(const QString& pMsg, QRegularExpressionMatch* pMatcher, int pTimeout)
{
	QCOMPARE(state(), QProcess::Running);

	const QRegularExpression regex(pMsg);
	mTimer.start(pTimeout);
	while (mTimer.isActive() || !mOutput.isEmpty())
	{
		if (mOutput.isEmpty())
		{
			mLoop.exec();
		}

		while (!mOutput.isEmpty())
		{
			const QString msg = mOutput.takeFirst();

			auto matcher = regex.match(msg);
			if (matcher.hasMatch())
			{
				mTimer.stop();
				if (pMatcher)
				{
					*pMatcher = matcher;
				}
				return;
			}
		}
	}

	QString notFound = QStringLiteral("Cannot find given output: ") + pMsg;
	QFAIL(notFound.toUtf8().constData());
}


void CliHelper::run()
{
	QCOMPARE(state(), QProcess::NotRunning);
	mServerPort = 0; // reset if we use same object again
	start();
	if (waitForStarted(TIMEOUT))
	{
		waitForOutput("Try to load UI plugin: \"UIPlugInCli\"");
	}

	QCOMPARE(state(), QProcess::Running);
}


void CliHelper::stop()
{
	terminate();
	closeWriteChannel();
	waitForFinished(TIMEOUT);
}


void CliHelper::waitForPong()
{
	send("ping");
	waitForOutput("^stdinput .*: \"ping\"");
	waitForOutput("Pong!");
}


quint16 CliHelper::getServerPort()
{
	if (mServerPort == 0)
	{
		send("port");
		QRegularExpressionMatch matcher;
		waitForOutput("^cli .*: Port: ([0-9]{1,5})$", &matcher);

		if (matcher.hasMatch() && !matcher.captured(1).isNull())
		{
			bool converted = false;
			auto port = matcher.captured(1).toUInt(&converted);
			if (converted && port <= USHRT_MAX)
			{
				mServerPort = static_cast<ushort>(port);
			}
		}
	}

	return mServerPort;
}


void CliHelper::tearDown()
{
	if (state() != QProcess::NotRunning)
	{
		stop();

		QCOMPARE(state(), QProcess::NotRunning);
		QCOMPARE(exitCode(), 0);
		if (state() != QProcess::NotRunning)
		{
			QProcess::close();
			QFAIL("kill application");
		}
	}
}
