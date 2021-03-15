/*!
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */


#include "PortFile.h"
#include "WebSocketHelper.h"

#include <QFile>
#include <QNetworkAccessManager>
#include <QProcess>
#include <QRegularExpression>
#include <QtTest>

using namespace governikus;


class test_UIPlugInQml
	: public QObject
{
	Q_OBJECT

	private:
		static const int PROCESS_TIMEOUT = 15000;

		QScopedPointer<QProcess> mApp2;
		QScopedPointer<WebSocketHelper> mHelper;

		QString getLogData()
		{
			QString logData;
			mHelper->sendMessage(QStringLiteral("{\"cmd\": \"GET_LOG\"}"));
			mHelper->waitForMessage([&logData](const QJsonObject& pMessage){
						if (pMessage["msg"] != "LOG")
						{
							return false;
						}

						auto jsonData = pMessage[QLatin1String("data")];
						if (!jsonData.isNull())
						{
							logData = jsonData.toString();
						}

						return true;
					});
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
			bool noQmlWarning = true;

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
			auto iterator = QRegularExpression(QStringLiteral("\n.* W .*\\.qml:.*\n")).globalMatch(logData);
			while (iterator.hasNext())
			{
				const QRegularExpressionMatch match = iterator.next();
				if (!match.captured(0).contains(QLatin1String("QML Connections: Implicitly defined onFoo properties in Connections are deprecated. Use this syntax instead:")))
				{
					noQmlWarning = false;
					break;
				}
			}
#else
			noQmlWarning = !logData.contains(QRegularExpression(" W .*\\.qml:"));
#endif

			bool success = initContainedAndSuccess && noQmlWarning;
			if (!success)
			{
				qDebug().noquote() << "Error output from AusweisApp2 process:\n" << logData;
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
			QTest::addColumn<QString>("platformSelector");

			QTest::newRow("Android") << QString("mobile,android,phone");
			QTest::newRow("Android Tablet") << QString("mobile,android,tablet");
			QTest::newRow("iOS") << QString("mobile,ios,phone");
			QTest::newRow("iOS Tablet") << QString("mobile,ios,tablet");
#ifdef Q_OS_WIN
			QTest::newRow("Windows") << QString("desktop,win");
#else
			QTest::newRow("macOS") << QString("desktop,nowin");
#endif
		}


		void test_qmlEngineInit()
		{
			QFETCH(QString, platformSelector);

			const QString& path = QCoreApplication::applicationDirPath() + "/../../src/";
			const QString& app = path + "AusweisApp2"
#ifdef Q_OS_WIN
					+ ".exe"
#endif
			;

			QStringList args;
			args << "--ui" << "qml";
			args << "--ui" << "websocket";
			args << "--port" << "0";
			args << "-platform" << "offscreen";

			mApp2.reset(new QProcess());
			mApp2->setProgram(app);
			mApp2->setWorkingDirectory(path);
			mApp2->setArguments(args);

			QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
			env.insert("OVERRIDE_PLATFORM_SELECTOR", platformSelector);
			env.insert("QT_QPA_OFFSCREEN_NO_GLX", QStringLiteral("1"));
			mApp2->setProcessEnvironment(env);

			mApp2->start();
			QVERIFY(mApp2->waitForStarted(PROCESS_TIMEOUT));

			QFile portInfoFile(PortFile::getPortFilename(QString(), mApp2->processId(), QStringLiteral("AusweisApp2")));
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

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("IDENTIFY"))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 1, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("IDENTIFY")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("SETTINGS"))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 2, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("SETTINGS")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("UPDATEINFORMATION"))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 3, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("UPDATEINFORMATION")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("PINMANAGEMENT"))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 4, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("PINMANAGEMENT")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("TUTORIAL"))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 5, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("TUTORIAL")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("HISTORY"))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 6, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("HISTORY")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("HELP"))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 7, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("HELP")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("PROVIDER"))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 8, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("PROVIDER")), PROCESS_TIMEOUT);

			accessManager.get(QNetworkRequest(QUrl(showUiUri.arg("SELF_AUTHENTICATION"))));
			QTRY_COMPARE_WITH_TIMEOUT(logSpy.size(), 9, PROCESS_TIMEOUT);
			QTRY_VERIFY_WITH_TIMEOUT(isShowUiInLog(QStringLiteral("SELF_AUTHENTICATION")), PROCESS_TIMEOUT);

			QVERIFY(isQmlEngineInitSuccess());
		}


		void cleanup()
		{
			const QString portFile = PortFile::getPortFilename(QString(), mApp2->processId(), QStringLiteral("AusweisApp2"));
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
				QWARN("Application didn't terminate.");
			}
#endif

			if (mApp2->state() != QProcess::NotRunning)
			{
#ifndef Q_OS_WIN
				QWARN("Killing application.");
#endif
				mApp2->kill();
			}
			QTRY_COMPARE_WITH_TIMEOUT(mApp2->state(), QProcess::NotRunning, PROCESS_TIMEOUT);
			if (mApp2->exitStatus() != QProcess::NormalExit)
			{
				const QByteArray stdError = mApp2->readAllStandardError();
				QWARN(stdError.constData());
				QVERIFY(!stdError.contains("ASSERT:"));
			}

#ifdef Q_OS_WIN
			QFile::remove(portFile);
#endif
			QVERIFY(!QFile::exists(portFile));
		}


};

QTEST_GUILESS_MAIN(test_UIPlugInQml)
#include "test_UIPlugInQml.moc"
