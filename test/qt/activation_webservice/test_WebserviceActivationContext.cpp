/*!
 * \brief Unit tests for \ref WebserviceActivationContext
 *
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
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


		void test_SetCommonHeaders()
		{
			HttpResponse response;
			WebserviceActivationContext context(mRequest);
			context.setCommonHeaders(response);

			QVERIFY(response.getHeaders().keys().contains(QByteArrayLiteral("Connection")));
			QVERIFY(response.getHeaders().keys().contains(QByteArrayLiteral("Cache-Control")));
			QVERIFY(response.getHeaders().keys().contains(QByteArrayLiteral("Pragma")));

			QMap<QByteArray, QByteArray> headers = response.getHeaders();
			QCOMPARE(headers.take(QByteArrayLiteral("Connection")), QByteArrayLiteral("close"));
			QCOMPARE(headers.take(QByteArrayLiteral("Cache-Control")), QByteArrayLiteral("no-cache, no-store"));
			QCOMPARE(headers.take(QByteArrayLiteral("Pragma")), QByteArrayLiteral("no-cache"));
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


};

QTEST_GUILESS_MAIN(test_WebserviceActivationContext)
#include "test_WebserviceActivationContext.moc"
