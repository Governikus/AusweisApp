/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "Env.h"
#include "states/StateBuilder.h"
#include "states/StateGenericSendReceive.h"

#include "MockNetworkManager.h"
#include "TestAuthContext.h"
#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;

Q_DECLARE_METATYPE(QSharedPointer<PaosMessage>)

class test_StateGenericSendReceive
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<StateSendInitializeFrameworkResponse> mState;
		QSharedPointer<AuthContext> mAuthContext;
		QSharedPointer<MockNetworkManager> mNetworkManager;

	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1.xml"));

			QSharedPointer<TcToken> tcToken(new TcToken(TestFileHelper::readFile(":/tctoken/ok.xml")));
			mAuthContext->setTcToken(tcToken);
			mNetworkManager.reset(new MockNetworkManager());
			Env::set(NetworkManager::staticMetaObject, mNetworkManager.data());

			mState.reset(StateBuilder::createState<StateSendInitializeFrameworkResponse>(mAuthContext));
			mState->setStateName("StateSendInitializeFrameworkResponse");
			mState->onEntry(nullptr);
		}


		void cleanup()
		{
			mState.clear();
			mAuthContext.clear();
		}


		void setReceivedMessage_data()
		{
			QTest::addColumn<PaosType>("type");
			QTest::addColumn<QString>("messageId");
			QTest::addColumn<QSharedPointer<PaosMessage> >("message");

			QSharedPointer<PaosMessage> tmp;

			tmp = QSharedPointer<StartPaosResponse>::create(QByteArray());
			QTest::newRow("startpaosResponse") << PaosType::STARTPAOS_RESPONSE << QStringLiteral("startpaosResponse") << tmp;

			tmp = QSharedPointer<InitializeFramework>::create(QByteArray());
			QTest::newRow("initializeFramework") << PaosType::INITIALIZE_FRAMEWORK << QStringLiteral("initializeFramework") << tmp;

			tmp = QSharedPointer<DIDList>::create(QByteArray());
			QTest::newRow("didList") << PaosType::DID_LIST << QStringLiteral("didList") << tmp;

			tmp = QSharedPointer<DIDAuthenticateEAC1>::create();
			QTest::newRow("didAuthenticateEac1") << PaosType::DID_AUTHENTICATE_EAC1 << QStringLiteral("didAuthenticateEac1") << tmp;

			tmp = QSharedPointer<DIDAuthenticateEAC2>::create();
			QTest::newRow("didAuthenticateEac2") << PaosType::DID_AUTHENTICATE_EAC2 << QStringLiteral("didAuthenticateEac2") << tmp;

			tmp = QSharedPointer<DIDAuthenticateEACAdditional>::create();
			QTest::newRow("didAuthenticateEacAdditionalInputType") << PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE << QStringLiteral("didAuthenticateEacAdditionalInputType") << tmp;

			tmp = QSharedPointer<Transmit>::create();
			QTest::newRow("transmit") << PaosType::TRANSMIT << QStringLiteral("transmit") << tmp;

			tmp = QSharedPointer<Disconnect>::create(QByteArray());
			QTest::newRow("disconnect") << PaosType::DISCONNECT << QStringLiteral("disconnect") << tmp;

			QTest::newRow("default") << PaosType::UNKNOWN << QStringLiteral("default") << QSharedPointer<PaosMessage>::create(PaosType::UNKNOWN);
		}


		void setReceivedMessage()
		{
			QFETCH(PaosType, type);
			QFETCH(QString, messageId);
			QFETCH(QSharedPointer<PaosMessage>, message);

			message->setMessageId(messageId);

			if (type == PaosType::UNKNOWN)
			{
				QTest::ignoreMessage(QtWarningMsg, "Unknown received message type: 0");
			}
			mState->setReceivedMessage(message);
			QCOMPARE(mAuthContext->getReceivedMessageId(), messageId);

			switch (type)
			{
				case PaosType::STARTPAOS_RESPONSE:
					QCOMPARE(mAuthContext->getStartPaosResponse(), message.staticCast<StartPaosResponse>());
					break;

				case PaosType::INITIALIZE_FRAMEWORK:
					QCOMPARE(mAuthContext->getInitializeFramework(), message.staticCast<InitializeFramework>());
					QVERIFY(mAuthContext->getInitializeFrameworkResponse());
					break;

				case PaosType::DID_LIST:
					QCOMPARE(mAuthContext->getDidList(), message.staticCast<DIDList>());
					QVERIFY(mAuthContext->getDidListResponse());
					break;

				case PaosType::DID_AUTHENTICATE_EAC1:
					QCOMPARE(mAuthContext->getDidAuthenticateEac1(), message.staticCast<DIDAuthenticateEAC1>());
					QVERIFY(mAuthContext->getDidAuthenticateResponseEac1());
					break;

				case PaosType::DID_AUTHENTICATE_EAC2:
					QCOMPARE(mAuthContext->getDidAuthenticateEac2(), message.staticCast<DIDAuthenticateEAC2>());
					QVERIFY(mAuthContext->getDidAuthenticateResponseEac2());
					break;

				case PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE:
					QCOMPARE(mAuthContext->getDidAuthenticateEacAdditional(), message.staticCast<DIDAuthenticateEACAdditional>());
					break;

				case PaosType::TRANSMIT:
					QVERIFY(mAuthContext->getTransmits().contains(message.staticCast<Transmit>()));
					QVERIFY(!mAuthContext->getTransmitResponses().isEmpty());
					break;

				case PaosType::DISCONNECT:
					QCOMPARE(mAuthContext->getDisconnect(), message.staticCast<Disconnect>());
					QVERIFY(mAuthContext->getDisconnectResponse());
					break;

				case PaosType::UNKNOWN:
					break;

				default:
					QFAIL("Wrong PaosType");
			}
		}


		void onPreSharedKeyAuthenticationRequired()
		{
			const QByteArray data("<?xml version=\"1.0\"?>"
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
			const QSharedPointer<TcToken> token(new TcToken(data));
			mAuthContext->setTcToken(token);
			QSslPreSharedKeyAuthenticator authenticator;

			QTest::ignoreMessage(QtDebugMsg, "pre-shared key authentication requested: \"\"");
			mState->onPreSharedKeyAuthenticationRequired(&authenticator);
			QCOMPARE(authenticator.identity(), QByteArray("1A2BB129"));
			QCOMPARE(authenticator.preSharedKey(), QByteArray::fromHex("4BC1A0B5"));
		}


		void connectionError()
		{
			MockNetworkReply* reply = new MockNetworkReply();
			reply->setNetworkError(QNetworkReply::ConnectionRefusedError, "forced connection refused");
			mNetworkManager->setNextReply(reply);
			QSharedPointer<InitializeFrameworkResponse> initializeFrameworkResponse(new InitializeFrameworkResponse());
			mAuthContext->setInitializeFrameworkResponse(initializeFrameworkResponse);

			QSignalSpy spy(mState.data(), &StateGenericSendReceive::fireAbort);
			QSignalSpy spyMock(mNetworkManager.data(), &MockNetworkManager::fireReply);

			mAuthContext->setStateApproved();
			QTRY_COMPARE(spyMock.count(), 1); // clazy:exclude=qstring-allocations
			mNetworkManager->fireFinished();

			QCOMPARE(spy.count(), 1);
		}


		void sendInitializeFrameworkResponse_receiveDIDList()
		{
			mNetworkManager->setFilename(":/paos/DIDList.xml");
			QSharedPointer<InitializeFrameworkResponse> initializeFrameworkResponse(new InitializeFrameworkResponse());
			mAuthContext->setInitializeFrameworkResponse(initializeFrameworkResponse);

			QSignalSpy spy(mState.data(), &StateGenericSendReceive::fireContinue);
			QSignalSpy spyMock(mNetworkManager.data(), &MockNetworkManager::fireReply);

			mAuthContext->setStateApproved();
			QTRY_COMPARE(spyMock.count(), 1); // clazy:exclude=qstring-allocations
			mNetworkManager->fireFinished();

			QCOMPARE(spy.count(), 1);
		}


		void sendInitializeFrameworkResponse_receiveDIDAuthenticateEAC1()
		{
			mNetworkManager->setFilename(":/paos/DIDAuthenticateEAC1.xml");
			QSharedPointer<InitializeFrameworkResponse> initializeFrameworkResponse(new InitializeFrameworkResponse());
			mAuthContext->setInitializeFrameworkResponse(initializeFrameworkResponse);

			QSignalSpy spy(mState.data(), &StateSendInitializeFrameworkResponse::fireReceivedExtractCvcsFromEac1InputType);
			QSignalSpy spyMock(mNetworkManager.data(), &MockNetworkManager::fireReply);

			mAuthContext->setStateApproved();
			QTRY_COMPARE(spyMock.count(), 1); // clazy:exclude=qstring-allocations
			mNetworkManager->fireFinished();

			QCOMPARE(spy.count(), 1);
		}


		void sendInitializeFrameworkResponse_unexpected()
		{
			mNetworkManager->setFilename(":/paos/Transmit.xml");
			QSharedPointer<InitializeFrameworkResponse> initializeFrameworkResponse(new InitializeFrameworkResponse());
			mAuthContext->setInitializeFrameworkResponse(initializeFrameworkResponse);

			QSignalSpy spy(mState.data(), &StateGenericSendReceive::fireAbort);
			QSignalSpy spyMock(mNetworkManager.data(), &MockNetworkManager::fireReply);

			mAuthContext->setStateApproved();
			QTRY_COMPARE(spyMock.count(), 1); // clazy:exclude=qstring-allocations
			mNetworkManager->fireFinished();

			QCOMPARE(spy.count(), 1);
		}


		void mappingToTrustedChannelError()
		{
			const QVector<GlobalStatus::Code> states = QVector<GlobalStatus::Code>()
					<< GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error
					<< GlobalStatus::Code::Workflow_TrustedChannel_Error_From_Server
					<< GlobalStatus::Code::Workflow_TrustedChannel_Hash_Not_In_Description
					<< GlobalStatus::Code::Workflow_TrustedChannel_No_Data_Received
					<< GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Certificate_Unsupported_Algorithm_Or_Length
					<< GlobalStatus::Code::Workflow_TrustedChannel_TimeOut
					<< GlobalStatus::Code::Workflow_TrustedChannel_Proxy_Error
					<< GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error
					<< GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error
					<< GlobalStatus::Code::Workflow_TrustedChannel_Other_Network_Error;

			for (const GlobalStatus::Code state : states)
			{
				const ECardApiResult& result = ECardApiResult(GlobalStatus(state));
				QCOMPARE(result.getMinor(), ECardApiResult::Minor::DP_Trusted_Channel_Establishment_Failed);
			}
		}


};

QTEST_GUILESS_MAIN(test_StateGenericSendReceive)
#include "test_StateGenericSendReceive.moc"
