/*!
 * \brief Unit tests for \ref UIPlugInWebSocket
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInWebSocket.h"
#include "WebSocketHelper.h"

#include <climits>
#include <QFile>
#include <QProcess>
#include <QtTest/QtTest>

using namespace governikus;


class test_UIPlugInWebSocket
	: public QObject
{
	Q_OBJECT

	private:
		static const int PROCESS_TIMEOUT = 15000;

		QScopedPointer<QProcess> mApp2;
		QScopedPointer<QWebSocket> mWebSocket;
		QScopedPointer<WebSocketHelper> mHelper;

	private Q_SLOTS:
		void initTestCase()
		{
			#ifdef Q_OS_WIN
			QSKIP("Not supported");
			#endif
		}


		void init()
		{
			qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");

			QString path = QCoreApplication::applicationDirPath() + "/../../src/";
			QString app = path + "AusweisApp2";
		#ifdef Q_OS_WIN
			app += ".exe";
		#endif

			QStringList args;
			args << "--ui" << "websocket";
			args << "--port" << "0";
			args << "--port-websocket" << "0";
		#ifndef Q_OS_WIN
			args << "-platform" << "offscreen";
		#endif

			mApp2.reset(new QProcess());
			mApp2->setProgram(app);
			mApp2->setWorkingDirectory(path);
			mApp2->setArguments(args);

			mApp2->start();
			mApp2->waitForStarted(PROCESS_TIMEOUT);
			QCOMPARE(mApp2->state(), QProcess::Running);

			QFile portInfoFile(WEBSOCKET_PORT_FILENAME(mApp2->processId()));
			for (int i = 0; i < PROCESS_TIMEOUT && portInfoFile.size() == 0; i += 200)
			{
				QThread::msleep(200);
			}
			QVERIFY(portInfoFile.size() != 0);
			QVERIFY(portInfoFile.open(QIODevice::ReadOnly));

			quint16 webSocketPort = 0;
			QTextStream(&portInfoFile) >> webSocketPort;
			QVERIFY(webSocketPort > 0);

			mHelper.reset(new WebSocketHelper(webSocketPort));
			QCOMPARE(mHelper->getState(), QAbstractSocket::SocketState::ConnectedState);
		}


		void cleanup()
		{
			mHelper.reset();

			mApp2->terminate();
			mApp2->waitForFinished(PROCESS_TIMEOUT);
			QCOMPARE(mApp2->state(), QProcess::NotRunning);
			QCOMPARE(mApp2->exitCode(), 0);

			if (mApp2->state() != QProcess::NotRunning)
			{
				mApp2->kill();
			}
		}


		void runAndStop()
		{
		}


		void getInfoAndApiLevel()
		{
			mHelper->sendMessage("{\"cmd\": \"GET_INFO\"}");
			QVERIFY(mHelper->waitForMessage([](const QJsonObject& pMessage){
						return pMessage["msg"] == "INFO" &&
						pMessage["VersionInfo"].toObject()["Name"] == "AusweisApp2";
					}));


			mHelper->sendMessage("{\"cmd\": \"GET_API_LEVEL\"}");
			QVERIFY(mHelper->waitForMessage([](const QJsonObject& pMessage){
						return pMessage["msg"] == "API_LEVEL" &&
						pMessage["available"].toArray().size() >= 1;
					}));
		}


		void tryLocalhostAuth()
		{
			#ifdef Q_OS_FREEBSD
			QSKIP("Not supported");
			#endif

			mHelper->sendMessage("{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\" : \"https://localhost/\"}");
			QVERIFY(mHelper->waitForMessage([](const QJsonObject& pMessage){
						return pMessage["msg"] == "AUTH";
					}));

			QVERIFY(mHelper->waitForMessage([](const QJsonObject& pMessage){
						return pMessage["result"].toObject()["major"].toString().endsWith("#error");
					}));
		}


};

QTEST_GUILESS_MAIN(test_UIPlugInWebSocket)
#include "test_UIPlugInWebSocket.moc"
