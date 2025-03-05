/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

#include <QProcess>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;

class test_CommandLineParser
	: public QObject
{
	Q_OBJECT

	private:
		static const int PROCESS_TIMEOUT = 30000;

	private Q_SLOTS:
		void checkHelp()
		{
#ifdef Q_OS_WIN
			QSKIP("Windows uses console or a dialog and won't be tested");
#endif

			const QString& path = QStringLiteral(AUSWEISAPP_BINARY_DIR);
			const QString& app = path + "AusweisApp"_L1;

			QStringList args;
			args << "--help"_L1;
			args << "-platform"_L1 << "offscreen"_L1;

			QProcess process;
			process.setProgram(app);
			process.setWorkingDirectory(path);
			process.setArguments(args);
			process.start();
			QVERIFY(process.waitForFinished(PROCESS_TIMEOUT));

			QStringList out;
			out << QStringLiteral("Usage: %1 [options]").arg(app);
			out << QString();
			out << QStringLiteral("Options:");
			out << QStringLiteral("  -h, --help                       Displays help on commandline options.");
#if (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
			out << QStringLiteral("  --help-all                       Displays help, including generic Qt options.");
#else
			out << QStringLiteral("  --help-all                       Displays help including Qt specific options.");
#endif
			out << QStringLiteral("  -v, --version                    Displays version information.");
			out << QStringLiteral("  --keep                           Keep logfile.");
			out << QStringLiteral("  --no-logfile                     Disable logfile.");
			out << QStringLiteral("  --no-loghandler                  Disable default log handler.");
			out << QStringLiteral("  --show                           Show window on startup.");
			out << QStringLiteral("  --no-proxy                       Ignore proxy settings.");
			out << QStringLiteral("  --ui <qml,webservice,websocket>  Use given UI plugin.");
			out << QStringLiteral("  --port <24727>                   Use listening port.");
			out << QStringLiteral("  --address <127.0.0.1,::1>        Use address binding.");
			out << QString();

			const auto& standardOut = process.readAllStandardOutput();
			const auto& expected = out.join('\n'_L1).toLocal8Bit();

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
