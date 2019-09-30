/*!
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */


#include "PortFile.h"
#include "WebSocketHelper.h"

#include <QFile>
#include <QProcess>
#include <QtTest>

using namespace governikus;


class test_UIPlugInQml
	: public QObject
{
	Q_OBJECT

	private:
		static const int PROCESS_TIMEOUT = 75000;

		QScopedPointer<QProcess> mApp2;
		QScopedPointer<WebSocketHelper> mHelper;

		bool isQmlEngineInitDone(const bool pCheckDoneAndSuccessful)
		{
			mHelper->sendMessage(QStringLiteral("{\"cmd\": \"GET_LOG\"}"));

			bool initContainedInLog = false;
			bool initContainedAndSuccess = false;
			QString logData;
			mHelper->waitForMessage([&initContainedInLog, &initContainedAndSuccess, &logData](const QJsonObject& pMessage){
						if (pMessage[QLatin1String("data")].isNull())
						{
							return false;
						}

						const QString& data = pMessage[QLatin1String("data")].toString();
						logData = data;
						initContainedInLog = data.contains(QLatin1String("QML engine initialization finished"));
						initContainedAndSuccess = data.contains(QLatin1String("QML engine initialization finished with 0 warnings."));
						return true;
					});

			if (pCheckDoneAndSuccessful)
			{
				if (initContainedAndSuccess)
				{
					return true;
				}

				qDebug().noquote() << "Error output from AusweisApp2 process:\n" << logData;
				return false;
			}

			return initContainedInLog;
		}


	private Q_SLOTS:
		void initTestCase()
		{
			qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");
		}


		void test_qmlEngineInit_data()
		{
			QTest::addColumn<QString>("platformSelector");

			QTest::newRow("Android") << QString("mobile,android,phone");
			QTest::newRow("Android Tablet") << QString("mobile,android,tablet");
			QTest::newRow("iOS") << QString("mobile,ios,phone");
			QTest::newRow("iOS Tablet") << QString("mobile,ios,tablet");
			QTest::newRow("macOS") << QString("desktop,mac");
			QTest::newRow("Windows") << QString("desktop,win");
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
			args << "-platform" << "minimal";

			mApp2.reset(new QProcess());
			mApp2->setProgram(app);
			mApp2->setWorkingDirectory(path);
			mApp2->setArguments(args);

			QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
			env.insert("OVERRIDE_PLATFORM_SELECTOR", platformSelector);
			mApp2->setProcessEnvironment(env);

			mApp2->start();
			QVERIFY(mApp2->waitForStarted(PROCESS_TIMEOUT));

			QFile portInfoFile(PortFile::getPortFilename(QString(), mApp2->processId(), QStringLiteral("AusweisApp2")));
			QTRY_COMPARE_WITH_TIMEOUT(portInfoFile.exists(), true, PROCESS_TIMEOUT);
			QVERIFY(portInfoFile.open(QIODevice::ReadOnly));

			quint16 webSocketPort = 0;
			QTextStream(&portInfoFile) >> webSocketPort;
			QVERIFY(webSocketPort > 0);

			mHelper.reset(new WebSocketHelper(webSocketPort));
			QCOMPARE(mHelper->getState(), QAbstractSocket::SocketState::ConnectedState);

			QTRY_VERIFY_WITH_TIMEOUT(isQmlEngineInitDone(false), PROCESS_TIMEOUT);
			QVERIFY(isQmlEngineInitDone(true));
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
				QWARN(mApp2->readAllStandardError().constData());
			}

#ifdef Q_OS_WIN
			QFile::remove(portFile);
#endif
			QVERIFY(!QFile::exists(portFile));
		}


};

QTEST_GUILESS_MAIN(test_UIPlugInQml)
#include "test_UIPlugInQml.moc"
