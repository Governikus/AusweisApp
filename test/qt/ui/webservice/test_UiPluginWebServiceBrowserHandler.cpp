/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginWebService.h"

#include "MockSocket.h"
#include "ResourceLoader.h"
#include "WorkflowRequest.h"
#include "context/AuthContext.h"

#include <QtCore>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_UiPluginWebServiceBrowserHandler
	: public QObject
{
	Q_OBJECT
	QPointer<MockSocket> mSocket;
	QSharedPointer<HttpRequest> mRequest;
	QSharedPointer<AuthContext> mAuthContext;

	QByteArray split(const QByteArray& pData)
	{
		return pData.split('\r').at(0);
	}

	private Q_SLOTS:
		void initTestCase()
		{
			//QCoreApplication::setApplicationVersion("1.0.0");
			HttpServer::cPort = 0;
		}


		void init()
		{
			mSocket = new MockSocket();
			mSocket->setSocketState(QAbstractSocket::ConnectedState);
			mRequest.reset(new HttpRequest(mSocket));
			mAuthContext.reset(new AuthContext(true, QUrl("http://activationUrl"_L1)));

			ResourceLoader::getInstance().init(); // html templates
		}


		void getActivationUrl()
		{
			mSocket->mReadBuffer = QByteArray("GET /testUrl HTTP/1.1\r\n"
											  "\r\n\r\n");
			Q_EMIT mSocket->readyRead();
			QCOMPARE(mRequest->getUrl().toEncoded(), QByteArray("/testUrl"));

			UiPluginWebService ui;

			QUrl url;
			connect(&ui, &UiPlugin::fireWorkflowRequested, this, [&url](const QSharedPointer<WorkflowRequest>& pRequest){
						url = pRequest->getContext().objectCast<AuthContext>()->getActivationUrl();
					});

			ui.handleWorkflowRequest(mRequest);
			QCOMPARE(url.toEncoded(), QByteArray("/testUrl"));
		}


		void connectionLost()
		{
			mSocket->setSocketState(QAbstractSocket::UnconnectedState);
			UiPluginWebService ui;

			// lost for error page
			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), QLatin1String("The browser connection was lost."));

			// lost for correct redirect
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->setRefreshUrl(QUrl("http://dummy?ResultMajor=ok"_L1));
			QTest::ignoreMessage(QtDebugMsg, "Perform redirect to URL: QUrl(\"http://dummy?ResultMajor=ok\")");
			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), "The connection to the browser was lost while redirecting to the provider (dummy). No automatic redirection could be performed."_L1);
		}


		void tcTokenNotFound()
		{
			UiPluginWebService ui;
			mAuthContext->setTcTokenNotFound(true);

			QTest::ignoreMessage(QtDebugMsg, "Send error page to browser, error code 404");
			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), QString());
			QCOMPARE(split(mSocket->mWriteBuffer), "HTTP/1.0 404 Not Found");
		}


		void internalErrorBeforeTcToken()
		{
			UiPluginWebService ui;
			mAuthContext->setStatus(GlobalStatus::Code::Workflow_InternalError_BeforeTcToken);

			QTest::ignoreMessage(QtDebugMsg, "Send error page to browser, error code 500");
			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), QString());
			QCOMPARE(split(mSocket->mWriteBuffer), "HTTP/1.0 500 Internal Server Error");
		}


		void emptyRefreshUrlNoTcToken()
		{
			UiPluginWebService ui;
			mAuthContext->setTcTokenNotFound(false);

			QTest::ignoreMessage(QtDebugMsg, "Invalid redirect URL: QUrl(\"\")");
			QTest::ignoreMessage(QtDebugMsg, "Send error page to browser, error code 400");
			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), QString());
			QCOMPARE(split(mSocket->mWriteBuffer), "HTTP/1.0 400 Bad Request");
		}


		void sendRefreshUrl()
		{
			UiPluginWebService ui;
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->setRefreshUrl(QUrl("http://dummy?ResultMajor=ok"_L1));

			QTest::ignoreMessage(QtDebugMsg, "Perform redirect to URL: QUrl(\"http://dummy?ResultMajor=ok\")");
			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), QString());
			QCOMPARE(split(mSocket->mWriteBuffer), "HTTP/1.0 303 See Other");
			QVERIFY(mSocket->mWriteBuffer.contains("Location: http://dummy?ResultMajor=ok"));
		}


};

QTEST_GUILESS_MAIN(test_UiPluginWebServiceBrowserHandler)
#include "test_UiPluginWebServiceBrowserHandler.moc"
