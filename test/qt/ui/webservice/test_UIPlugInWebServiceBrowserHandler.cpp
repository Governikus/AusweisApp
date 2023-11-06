/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInWebService.h"

#include "MockSocket.h"
#include "ResourceLoader.h"
#include "WorkflowRequest.h"
#include "context/AuthContext.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_UIPlugInWebServiceBrowserHandler
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
			mAuthContext.reset(new AuthContext(true, QUrl("http://activationUrl")));

			ResourceLoader::getInstance().init(); // html templates
		}


		void getActivationUrl()
		{
			mSocket->mReadBuffer = QByteArray("GET /testUrl HTTP/1.1\r\n"
											  "\r\n\r\n");
			Q_EMIT mSocket->readyRead();
			QCOMPARE(mRequest->getUrl().toEncoded(), QByteArray("/testUrl"));

			UIPlugInWebService ui;

			QUrl url;
			connect(&ui, &UIPlugIn::fireWorkflowRequested, this, [&url](const QSharedPointer<WorkflowRequest>& pRequest){
					url = pRequest->getContext().objectCast<AuthContext>()->getActivationUrl();
				});

			ui.handleWorkflowRequest(mRequest);
			QCOMPARE(url.toEncoded(), QByteArray("/testUrl"));
		}


		void connectionLost()
		{
			mSocket->setSocketState(QAbstractSocket::UnconnectedState);
			UIPlugInWebService ui;

			// lost for error page
			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), QLatin1String("The browser connection was lost."));

			// lost for correct redirect
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->setRefreshUrl(QUrl("http://dummy"));
			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), QLatin1String("The connection to the browser was lost. No forwarding was executed. Please try to call the URL again manually: <a href='http://dummy?ResultMajor=ok'>dummy</a>"));
		}


		void tcTokenNotFound()
		{
			UIPlugInWebService ui;
			mAuthContext->setTcTokenNotFound(true);

			QTest::ignoreMessage(QtDebugMsg, "Send error page to browser, error code 404");
			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), QString());
			QCOMPARE(split(mSocket->mWriteBuffer), "HTTP/1.0 404 Not Found");
		}


		void internalErrorBeforeTcToken()
		{
			UIPlugInWebService ui;
			mAuthContext->setStatus(GlobalStatus::Code::Workflow_InternalError_BeforeTcToken);

			QTest::ignoreMessage(QtDebugMsg, "Send error page to browser, error code 500");
			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), QString());
			QCOMPARE(split(mSocket->mWriteBuffer), "HTTP/1.0 500 Internal Server Error");
		}


		void emptyRefreshUrlNoTcToken()
		{
			UIPlugInWebService ui;
			mAuthContext->setTcTokenNotFound(false);

			QTest::ignoreMessage(QtDebugMsg, "Send error page to browser, error code 400");
			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), QString());
			QCOMPARE(split(mSocket->mWriteBuffer), "HTTP/1.0 400 Bad Request");
		}


		void emptyRefreshUrlNoCommunicationErrorAddress()
		{
			const QByteArray tokenData("<?xml version=\"1.0\"?>"
									   "<TCTokenType>"
									   "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
									   "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
									   "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
									   "  <Binding> urn:liberty:paos:2006-08 </Binding>"
									   "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
									   "  <PathSecurity-Parameters>"
									   "    <PSK> 4BC1A0B5 </PSK>"
									   "  </PathSecurity-Parameters>"
									   "</TCTokenType>");

			UIPlugInWebService ui;
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->setTcToken(QSharedPointer<TcToken>::create(tokenData));

			QTest::ignoreMessage(QtDebugMsg, "Send error page to browser, error code 400");
			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), QString());
			QCOMPARE(split(mSocket->mWriteBuffer), "HTTP/1.0 400 Bad Request");
		}


		void emptyRefreshUrl()
		{
			const QByteArray tokenData("<?xml version=\"1.0\"?>"
									   "<TCTokenType>"
									   "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
									   "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
									   "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
									   "  <CommunicationErrorAddress>https://flupp</CommunicationErrorAddress>"
									   "  <Binding> urn:liberty:paos:2006-08 </Binding>"
									   "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
									   "  <PathSecurity-Parameters>"
									   "    <PSK> 4BC1A0B5 </PSK>"
									   "  </PathSecurity-Parameters>"
									   "</TCTokenType>");

			UIPlugInWebService ui;
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->setTcToken(QSharedPointer<TcToken>::create(tokenData));

			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), QString());
			QCOMPARE(split(mSocket->mWriteBuffer), "HTTP/1.0 303 See Other");
			QVERIFY(mSocket->mWriteBuffer.contains("Location: https://flupp"));
		}


		void sendRefreshUrl()
		{
			UIPlugInWebService ui;
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->setRefreshUrl(QUrl("http://dummy"));

			QTest::ignoreMessage(QtDebugMsg, "Redirect URL: QUrl(\"http://dummy?ResultMajor=ok\")");
			QCOMPARE(ui.sendRedirect(mRequest, mAuthContext), QString());
			QCOMPARE(split(mSocket->mWriteBuffer), "HTTP/1.0 303 See Other");
			QVERIFY(mSocket->mWriteBuffer.contains("Location: http://dummy?ResultMajor=ok"));
		}


};

QTEST_GUILESS_MAIN(test_UIPlugInWebServiceBrowserHandler)
#include "test_UIPlugInWebServiceBrowserHandler.moc"
