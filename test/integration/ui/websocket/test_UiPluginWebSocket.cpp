/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

#include "PortFile.h"
#include "WebSocketHelper.h"

#include <QFile>
#include <QProcess>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_UiPluginWebSocket
	: public QObject
{
	Q_OBJECT

	private:
		static const int PROCESS_TIMEOUT = 30000;

		QScopedPointer<QProcess> mApp2;
		QScopedPointer<WebSocketHelper> mHelper;
		quint16 mWebSocketPort = 0;

		void connect()
		{
			mHelper.reset(new WebSocketHelper(mWebSocketPort));
			QTRY_VERIFY_WITH_TIMEOUT(mHelper->isConnected(), PROCESS_TIMEOUT);
		}

	private Q_SLOTS:
		void initTestCase()
		{
			qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");
		}


		void init()
		{
			QString path = QStringLiteral(AUSWEISAPP_BINARY_DIR);
			QString app = path + "AusweisApp"_L1;
#ifdef Q_OS_WIN
			app += ".exe"_L1;
#endif

			QStringList args;
			args << "--ui"_L1 << "websocket"_L1;
			args << "--port"_L1 << "0"_L1;
#ifndef Q_OS_WIN
			args << "-platform"_L1 << "offscreen"_L1;
#endif

			mApp2.reset(new QProcess());
			mApp2->setProgram(app);
			mApp2->setWorkingDirectory(path);
			mApp2->setArguments(args);
			mApp2->setProcessChannelMode(QProcess::ForwardedChannels);

			QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
			env.insert(QStringLiteral("AUSWEISAPP_WEBSOCKET_ORIGIN"), QStringLiteral(".*Origin"));
			mApp2->setProcessEnvironment(env);

			mApp2->start();
			mApp2->waitForStarted(PROCESS_TIMEOUT);
			QCOMPARE(mApp2->state(), QProcess::Running);

			QFile portInfoFile(PortFile::getPortFilename(QString(), mApp2->processId(), QStringLiteral("AusweisApp")));
			QTRY_COMPARE_WITH_TIMEOUT(portInfoFile.exists(), true, PROCESS_TIMEOUT); // clazy:exclude=qstring-allocations
			QVERIFY(portInfoFile.open(QIODevice::ReadOnly));

			QTextStream(&portInfoFile) >> mWebSocketPort;
			QVERIFY(mWebSocketPort > 0);
		}


		void cleanup()
		{
			const QString portFile = PortFile::getPortFilename(QString(), mApp2->processId(), QStringLiteral("AusweisApp"));
			QVERIFY(QFile::exists(portFile));
			mHelper.reset();

			QCOMPARE(mApp2->state(), QProcess::Running);

#ifndef Q_OS_WIN
			// QProcess::terminate() sends WM_CLOSE on Windows. We can not handle this signal
			// since it does not clearly indicate a quit request. It might simply be a closing
			// window, too.
			mApp2->terminate();
			mApp2->waitForFinished(PROCESS_TIMEOUT);
#endif

			if (mApp2->state() != QProcess::NotRunning)
			{
				mApp2->kill();
			}
			QTRY_COMPARE_WITH_TIMEOUT(mApp2->state(), QProcess::NotRunning, PROCESS_TIMEOUT); // clazy:exclude=qstring-allocations

#ifndef Q_OS_WIN
			// There will never be a clean shutdown on Windows.
			if (mApp2->exitCode() != 0)
			{
				qDebug().noquote() << "Error output from AusweisApp process:\n" << mApp2->readAllStandardError();
			}
			QCOMPARE(mApp2->exitCode(), 0);
#endif

#ifdef Q_OS_WIN
			QFile::remove(portFile);
#endif
			QVERIFY(!QFile::exists(portFile));
		}


		void origin_data()
		{
			QTest::addColumn<QByteArray>("origin");
			QTest::addColumn<bool>("forbidden");

			QTest::newRow("forbidden") << QByteArray("TEST") << true;
			QTest::newRow("empty") << QByteArray("") << true;
			QTest::newRow("null") << QByteArray("null") << true;
			QTest::newRow("allowed") << QByteArray("MyOrigin") << false;
		}


		void origin()
		{
			QFETCH(QByteArray, origin);
			QFETCH(bool, forbidden);

			mHelper.reset(new WebSocketHelper(mWebSocketPort, origin));

			if (forbidden)
			{
				QCOMPARE(mHelper->getState(), QAbstractSocket::ConnectingState);
				QTRY_COMPARE(mHelper->getState(), QAbstractSocket::UnconnectedState);
				QCOMPARE(mHelper->getError(), QAbstractSocket::ConnectionRefusedError);
			}
			else
			{
				QTRY_VERIFY_WITH_TIMEOUT(mHelper->isConnected(), PROCESS_TIMEOUT);
			}
		}


		void runAndStop()
		{
			connect();
		}


		void getInfoAndApiLevel()
		{
			connect();

			mHelper->sendMessage("{\"cmd\": \"GET_INFO\"}"_L1);
			QVERIFY(mHelper->waitForMessage([](const QJsonObject& pMessage){
						return pMessage["msg"_L1] == "INFO"_L1 &&
							   pMessage["VersionInfo"_L1].toObject()["Name"_L1] == QLatin1String("AusweisApp2");
					}));

			mHelper->sendMessage("{\"cmd\": \"GET_API_LEVEL\"}"_L1);
			QVERIFY(mHelper->waitForMessage([](const QJsonObject& pMessage){
						return pMessage["msg"_L1] == "API_LEVEL"_L1 &&
							   pMessage["available"_L1].toArray().size() >= 1;
					}));
		}


		void tryLocalhostAuth()
		{
#ifdef Q_OS_FREEBSD
			QSKIP("Not supported");
#endif

			connect();
			mHelper->sendMessage("{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\" : \"https://localhost/\"}"_L1);
			QVERIFY(mHelper->waitForMessage([](const QJsonObject& pMessage){
						return pMessage["msg"_L1] == "AUTH"_L1;
					}));

			QVERIFY(mHelper->waitForMessage([](const QJsonObject& pMessage){
						return pMessage["result"_L1].toObject()["major"_L1].toString().endsWith("#error"_L1);
					}));
		}


};

QTEST_GUILESS_MAIN(test_UiPluginWebSocket)
#include "test_UiPluginWebSocket.moc"
