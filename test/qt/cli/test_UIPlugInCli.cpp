/*!
 * \brief Unit tests for \ref UIPlugInCli
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "CliHelper.h"

#include <QtTest/QtTest>

using namespace governikus;

class test_UIPlugInCli
	: public QObject
{
	Q_OBJECT

	private:
		QScopedPointer<CliHelper> cli;

	private Q_SLOTS:
		void initTestCase()
		{
			#ifdef Q_OS_WIN
			QSKIP("Console is not supported at the moment");
			#endif

			#ifdef Q_OS_BSD4
			QSKIP("Platform plugin seems broken");
			#endif

			QSKIP("QProcess/CliHelper is flaky");
		}


		void init()
		{
			cli.reset(new CliHelper());
		}


		void cleanup()
		{
			cli->tearDown();

		}


		void quit()
		{
			CLI_VERIFY(cli->run());
			CLI_VERIFY(cli->waitForPong());
			cli->send("help");
			CLI_VERIFY(cli->waitForOutput("Available commands:"));
			cli->send("quit");
			CLI_VERIFY(cli->waitForOutput("Shutdown application"));
			CLI_VERIFY(cli->waitForOutput("Emit fire shutdown"));
			CLI_VERIFY(cli->waitForOutput("Shutdown ReaderManager"));
			CLI_VERIFY(cli->waitForOutput("Unregister resource:"));
			CLI_VERIFY(cli->waitForOutput("Quit event loop of QCoreApplication"));
			cli->waitForFinished();
			QCOMPARE(cli->state(), QProcess::NotRunning);
			QCOMPARE(cli->exitCode(), 0);
		}


		void term()
		{
			CLI_VERIFY(cli->run());
			CLI_VERIFY(cli->waitForPong());
			cli->stop();
			QCOMPARE(cli->state(), QProcess::NotRunning);
			QCOMPARE(cli->exitCode(), 0);
		}


		void termDuringOldPin()
		{
			CLI_VERIFY(cli->run());
			cli->send("changepin");
			CLI_VERIFY(cli->waitForOutput("Please enter old PIN"));
			cli->stop();
			QCOMPARE(cli->state(), QProcess::NotRunning);
			QCOMPARE(cli->exitCode(), 0);
		}


		void changePin()
		{
			QSKIP("unfinished");

			CLI_VERIFY(cli->run());
			cli->send("changepin");
			CLI_VERIFY(cli->waitForOutput("Please enter old PIN"));
			cli->send("123456");
			CLI_VERIFY(cli->waitForOutput("Please enter new PIN"));
			cli->send("123456");
			CLI_VERIFY(cli->waitForOutput("Please enter new PIN again"));
			cli->send("123456");
			CLI_VERIFY(cli->waitForOutput("Start"));
		}


		void getServerPort()
		{
			CLI_VERIFY(cli->run());
			QVERIFY(cli->getServerPort() > 0);
		}


};

QTEST_GUILESS_MAIN(test_UIPlugInCli)
#include "test_UIPlugInCli.moc"
