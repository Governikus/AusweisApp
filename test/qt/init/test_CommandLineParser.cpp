/*!
 * \brief Unit tests for \ref CommandLineParser
 *
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

#include "CommandLineParser.h"

#include <QProcess>
#include <QtTest>

using namespace governikus;

class test_CommandLineParser
	: public QObject
{
	Q_OBJECT

	private:
		static const int PROCESS_TIMEOUT = 5000;

	private Q_SLOTS:
		void checkHelp()
		{
			#ifdef Q_OS_WIN
			QSKIP("Windows uses console or a dialog and won't be tested");
			#endif

			const QString& path = QCoreApplication::applicationDirPath() + "/../../src/AusweisApp2";

			QStringList args;
			args << "--help";
			args << "-platform" << "offscreen";

			QProcess process;
			process.setProgram(path);
			process.setArguments(args);
			process.start();
			QVERIFY(process.waitForFinished(PROCESS_TIMEOUT));

			QStringList out;
			out << QStringLiteral("Usage: %1 [options]").arg(path);
			out << QString();
			out << QStringLiteral("Options:");
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
			out << QStringLiteral("  -h, --help            Displays help on commandline options.");
			out << QStringLiteral("  --help-all            Displays help including Qt specific options.");
#else
			out << QStringLiteral("  -h, --help            Displays this help.");
#endif
			out << QStringLiteral("  -v, --version         Displays version information.");
			out << QStringLiteral("  --keep                Keep logfile.");
			out << QStringLiteral("  --no-logfile          Disable logfile.");
			out << QStringLiteral("  --no-loghandler       Disable default log handler.");
			out << QStringLiteral("  --show                Show window on startup.");
			out << QStringLiteral("  --no-proxy            Ignore proxy settings.");
			out << QStringLiteral("  --ui <Qml,WebSocket>  Use given UI plugin.");
			out << QStringLiteral("  --port <24727>        Use listening port.");
			out << QString();

			const auto& standardOut = process.readAllStandardOutput();
			const auto& expected = out.join(QChar('\n')).toLocal8Bit();

			// helps to debug if QCOMPARE fails
			qDebug().noquote() << '\n' << standardOut;
			qDebug().noquote() << '\n' << expected;

			QCOMPARE(process.exitStatus(), QProcess::NormalExit);
			QCOMPARE(standardOut, expected);
			QCOMPARE(process.readAllStandardError(), QByteArray());
		}


};

QTEST_GUILESS_MAIN(test_CommandLineParser)
#include "test_CommandLineParser.moc"
