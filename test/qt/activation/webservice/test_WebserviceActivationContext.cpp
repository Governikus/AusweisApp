/*!
 * \brief Unit tests for \ref WebserviceActivationContext
 *
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "WebserviceActivationContext.h"

#include "LogHandler.h"
#include "MockSocket.h"
#include "ResourceLoader.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_WebserviceActivationContext
	: public QObject
{
	Q_OBJECT
	QPointer<MockSocket> mSocket;
	QSharedPointer<HttpRequest> mRequest;

	private Q_SLOTS:
		void initTestCase()
		{
			mSocket = new MockSocket();
			mRequest.reset(new HttpRequest(mSocket));

			ResourceLoader::getInstance().init();
		}


		void test_GetActivationUrl()
		{
			WebserviceActivationContext context(mRequest);
			const QByteArray testUrl("testUrl");
			mRequest->mUrl = testUrl;
			QCOMPARE(context.getActivationURL().toEncoded(), testUrl);
		}


		void test_SendProcessing()
		{
			WebserviceActivationContext context(mRequest);
			QVERIFY(!context.sendProcessing());
			QCOMPARE(context.getSendError(), QString("The browser connection was lost."));

			mSocket->setSocketState(QAbstractSocket::ConnectedState);
			QVERIFY(context.sendProcessing());
		}


		void test_SendActivationAlreadyActive()
		{
			WebserviceActivationContext context(mRequest);

			mSocket->setSocketState(QAbstractSocket::UnconnectedState);
			QVERIFY(!context.sendOperationAlreadyActive());
			QCOMPARE(context.getSendError(), QString("The browser connection was lost."));

			mSocket->setSocketState(QAbstractSocket::ConnectedState);
			QVERIFY(context.sendOperationAlreadyActive());
		}


		void test_SendRedirect()
		{
			WebserviceActivationContext context(mRequest);

			mSocket->setSocketState(QAbstractSocket::UnconnectedState);
			QTest::ignoreMessage(QtDebugMsg, "Redirect URL: QUrl(\"?ResultMajor=ok\")");
			QVERIFY(!context.sendRedirect(QUrl(), GlobalStatus::Code::No_Error));
			QCOMPARE(context.getSendError(), QString("The connection to the browser was lost. No forwarding was executed. Please try to call the URL again manually: <a href='?ResultMajor=ok'></a>"));

			mSocket->setSocketState(QAbstractSocket::ConnectedState);
			QVERIFY(context.sendRedirect(QUrl(), GlobalStatus::Code::No_Error));
		}


		void test_SendErrorPage()
		{
			WebserviceActivationContext context(mRequest);

			QTest::ignoreMessage(QtDebugMsg, "Send error page to browser, error code 400");
			QVERIFY(context.sendErrorPage(HTTP_STATUS_BAD_REQUEST, GlobalStatus::Code::Unknown_Error));
			const auto buffer = mSocket->mWriteBuffer;
			QVERIFY(buffer.contains("Connection: close"));
			QVERIFY(buffer.contains("Cache-Control: no-cache, no-store"));
			QVERIFY(buffer.contains("Pragma: no-cache"));

			mSocket->setSocketState(QAbstractSocket::UnconnectedState);
			QVERIFY(!context.sendErrorPage(HTTP_STATUS_BAD_REQUEST, GlobalStatus::Code::Unknown_Error));
		}


};

QTEST_GUILESS_MAIN(test_WebserviceActivationContext)
#include "test_WebserviceActivationContext.moc"
