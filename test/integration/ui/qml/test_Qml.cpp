/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */


#include "PortFile.h"
#include "WebSocketHelper.h"

#include <QFile>
#include <QNetworkAccessManager>
#include <QProcess>
#include <QRegularExpression>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_Qml
	: public QObject
{
	Q_OBJECT

	private:
		static const int PROCESS_TIMEOUT = 30000;

		QScopedPointer<QProcess> mApp2;
		QScopedPointer<WebSocketHelper> mHelper;

		QString getLogData()
		{
			QString logData;
			mHelper->sendMessage(QStringLiteral("{\"cmd\": \"GET_LOG\"}"));
			if (!mHelper->waitForMessage([&logData](const QJsonObject& pMessage){
						if (pMessage["msg"_L1] != "LOG"_L1)
						{
							return false;
						}

						auto jsonData = pMessage["data"_L1];
						if (!jsonData.isNull())
						{
							logData = jsonData.toString();
						}

						return true;
					}))
			{
				qCritical() << "Cannot fetch message";
			}

			return logData;
		}


		bool isQmlEngineInitDone()
		{
			return getLogData().contains(QStringLiteral("QML engine initialization finished"));
		}


		bool isQmlEngineInitSuccess()
		{
			const QString logData = getLogData();

			bool initContainedAndSuccess = logData.contains(QLatin1String("QML engine initialization finished with 0 warnings."));
			bool noQmlWarning = !logData.contains(QRegularExpression(" W .*\\.qml:"_L1));
			bool success = initContainedAndSuccess && noQmlWarning;
			if (!success)
			{
				qDebug().noquote() << "Error output from AusweisApp process:\n" << logData;
			}
			return success;
		}


		bool isShowUiInLog(const QString& showUi)
		{
			return getLogData().contains(QStringLiteral(" \"GET\" | QUrl(\"/eID-Client?showui=%1\")").arg(showUi));
		}

	private Q_SLOTS:
		void initTestCase()
		{
			qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");
			qRegisterMetaType<QNetworkReply*>("QNetworkReply*");
		}


		void test_qmlEngineInit_data()
		{
			QTest::addColumn<QString>("platform");

			QTest::newRow("Android") << u"android"_s;
			QTest::newRow("iOS") << u"ios"_s;
			QTest::newRow("Desktop") << u""_s;
		}


		void test_qmlEngineInit()
		{
			QFETCH(QString, platform);

			QString path = QStringLiteral(AUSWEISAPP_BINARY_DIR);
			QString app = path + "AusweisApp"_L1;
#ifdef Q_OS_WIN
			app += ".exe"_L1;
#endif

			QStringList args;
			args << "--ui"_L1 << "qml"_L1;
			args << "--ui"_L1 << "websocket"_L1;
			args << "--port"_L1 << "0"_L1;
			args << "-platform"_L1 << "offscreen"_L1;

			mApp2.reset(new QProcess());
			mApp2->setProgram(app);
			mApp2->setWorkingDirectory(path);
			mApp2->setArguments(args);

			QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
			env.insert("OVERRIDE_PLATFORM"_L1, platform);
			env.insert("QT_QPA_OFFSCREEN_NO_GLX"_L1, "1"_L1);
			env.insert("QML_DISABLE_DISK_CACHE"_L1, "1"_L1);
			mApp2->setProcessEnvironment(env);

			mApp2->start();
			QVERIFY(mApp2->waitForStarted(PROCESS_TIMEOUT));

			QFile portInfoFile(PortFile::getPortFilename(QString(), mApp2->processId(), QStringLiteral("AusweisApp")));
			QTRY_COMPARE_WITH_TIMEOUT(portInfoFile.exists(), true, PROCESS_TIMEOUT); // clazy:exclude=qstring-allocations
			QVERIFY(portInfoFile.open(QIODevice::ReadOnly));

			quint16 applicationPort = 0;
			QTextStream(&portInfoFile) >> applicationPort;
			QVERIFY(applicationPort > 0);

			mHelper.reset(new WebSocketHelper(applicationPort));
			QTRY_VERIFY_WITH_TIMEOUT(mHelper->isConnected(), PROCESS_TIMEOUT); // clazy:exclude=qstring-allocations

			QTRY_VERIFY_WITH_TIMEOUT(!getLogData().isEmpty(), PROCESS_TIMEOUT); // clazy:exclude=qstring-allocations
			QTRY_VERIFY_WITH_TIMEOUT(isQmlEngineInitDone(), PROCESS_TIMEOUT); // clazy:exclude=qstring-allocations
			QVERIFY(isQmlEngineInitSuccess());

			const QString showUiUri = QStringLiteral("http://localhost:%1/eID-Client?showui=%2").arg(applicationPort);
			QNetworkAccessManager accessManager;
			QSignalSpy logSpy(&accessManager, &QNetworkAccessManager::finished);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("IDENTIFY"_L1))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 1, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("IDENTIFY")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("SETTINGS"_L1))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 2, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("SETTINGS")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("UPDATEINFORMATION"_L1))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 3, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("UPDATEINFORMATION")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("PINMANAGEMENT"_L1))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 4, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("PINMANAGEMENT")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("ONBOARDING"_L1))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 5, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("ONBOARDING")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("HELP"_L1))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 6, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("HELP")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("PROVIDER"_L1))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 7, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("PROVIDER")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("SELF_AUTHENTICATION"_L1))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 8, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("SELF_AUTHENTICATION")), PROCESS_TIMEOUT);

			QVERIFY(isQmlEngineInitSuccess());
		}


		void cleanup()
		{
			const QString portFile = PortFile::getPortFilename(QString(), mApp2->processId(), QStringLiteral("AusweisApp"));
			QVERIFY(QFile::exists(portFile));
			mHelper.reset();

			if (mApp2->state() == QProcess::NotRunning)
			{
				return;
			}

#ifndef Q_OS_WIN
			// QProcess::terminate() sends WM_CLOSE on Windows. We can not handle this signal
			// since it does not clearly indicate a quit request. It might simply be a closing
			// window, too.
			mApp2->terminate();
			if (!mApp2->waitForFinished(PROCESS_TIMEOUT))
			{
				qCritical() << "Application didn't terminate.";
			}
#endif

			if (mApp2->state() != QProcess::NotRunning)
			{
#ifndef Q_OS_WIN
				qCritical() << "Killing application.";
#endif
				mApp2->kill();
			}
			QTRY_COMPARE_WITH_TIMEOUT(mApp2->state(), QProcess::NotRunning, PROCESS_TIMEOUT);
			if (mApp2->exitStatus() != QProcess::NormalExit)
			{
				const QByteArray stdError = mApp2->readAllStandardError();
				qCritical() << stdError;
				QVERIFY(!stdError.contains("ASSERT:"));
			}

#ifdef Q_OS_WIN
			QFile::remove(portFile);
#endif
			QVERIFY(!QFile::exists(portFile));
		}


};

QTEST_GUILESS_MAIN(test_Qml)
#include "test_Qml.moc"
