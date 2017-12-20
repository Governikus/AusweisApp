/*!
 * \brief Helper to test app via CliPlugIn.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QProcess>
#include <QRegularExpression>
#include <QStringList>
#include <QtTest/QtTest>
#include <QTimer>

#define CLI_VERIFY(cmd) cmd; QVERIFY(!QTest::currentTestFailed())

namespace governikus
{

class CliHelper
	: public QProcess
{
	Q_OBJECT

	private:
		QTimer mTimer;
		QEventLoop mLoop;
		QStringList mOutput;
		quint16 mServerPort;

	private Q_SLOTS:
		void storeOutput();

	public:
		static const int TIMEOUT = 15000;
		CliHelper(QObject* pParent = nullptr);
		virtual ~CliHelper();

		void send(const char* pMsg)
		{
			write(pMsg);
			write("\n");
			if (bytesToWrite())
			{
				QVERIFY(waitForBytesWritten(TIMEOUT));
			}
		}


		void waitForOutput(const QString& pMsgList, QRegularExpressionMatch* pMatcher = nullptr, int pTimeout = TIMEOUT);
		void waitForPong();
		void run();
		void stop();
		void tearDown();
		quint16 getServerPort();
};

} /* namespace governikus */
