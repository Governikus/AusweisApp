/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref UIPlugInWebSocket
 */

#include "PortFile.h"
#include "WebSocketHelper.h"

#include <QFile>
#include <QProcess>
#include <QtTest>

using namespace governikus;


class test_UIPlugInWebSocket
	: public QObject
{
	Q_OBJECT

	private:
		static const int PROCESS_TIMEOUT = 30000;

		QScopedPointer<QProcess> mApp2;
		QScopedPointer<WebSocketHelper> mHelper;

	private Q_SLOTS:
		void initTestCase()
		{
			qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");
		}


		void init()
		{
			QString path = QStringLiteral(AUSWEISAPP_BINARY_DIR);
			QString app = path + "AusweisApp";
#ifdef Q_OS_WIN
			app += ".exe";
#endif

			QStringList args;
			args << "--ui" << "websocket";
			args << "--port" << "0";
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

			QFile portInfoFile(PortFile::getPortFilename(QString(), mApp2->processId(), QStringLiteral("AusweisApp")));
			QTRY_COMPARE_WITH_TIMEOUT(portInfoFile.exists(), true, PROCESS_TIMEOUT); // clazy:exclude=qstring-allocations
			QVERIFY(portInfoFile.open(QIODevice::ReadOnly));

			quint16 webSocketPort = 0;
			QTextStream(&portInfoFile) >> webSocketPort;
			QVERIFY(webSocketPort > 0);

			mHelper.reset(new WebSocketHelper(webSocketPort));
			QTRY_VERIFY_WITH_TIMEOUT(mHelper->isConnected(), PROCESS_TIMEOUT);
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


		void runAndStop()
		{
		}


		void getInfoAndApiLevel()
		{
			mHelper->sendMessage("{\"cmd\": \"GET_INFO\"}");
			QVERIFY(mHelper->waitForMessage([](const QJsonObject& pMessage){
					return pMessage["msg"] == "INFO" &&
						   pMessage["VersionInfo"].toObject()["Name"] == QLatin1String("AusweisApp2");
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
