/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "Env.h"
#include "states/StateBuilder.h"
#include "states/StateGenericSendReceive.h"

#include "MockNetworkManager.h"
#include "TestAuthContext.h"

#include <QList>
#include <QPair>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_DECLARE_METATYPE(QSharedPointer<PaosMessage>)
using Pair = QPair<QByteArray, QByteArray>;
Q_DECLARE_METATYPE(QList<Pair>)

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
			mAuthContext.reset(new TestAuthContext(":/paos/DIDAuthenticateEAC1.xml"_L1));

			const QByteArray tokenData("<?xml version=\"1.0\"?>"
									   "<TCTokenType>"
									   "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
									   "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
									   "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
									   "  <CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>"
									   "  <Binding> urn:liberty:paos:2006-08 </Binding>"
									   "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
									   "  <PathSecurity-Parameters>"
									   "    <PSK> 4BC1A0B5 </PSK>"
									   "  </PathSecurity-Parameters>"
									   "</TCTokenType>");
			QSharedPointer<TcToken> tcToken(new TcToken(tokenData));
			mAuthContext->setTcToken(tcToken);
			mNetworkManager.reset(new MockNetworkManager());
			Env::set(NetworkManager::staticMetaObject, mNetworkManager.data());

			mState.reset(StateBuilder::createState<StateSendInitializeFrameworkResponse>(mAuthContext));
			mState->onEntry(nullptr);
		}


		void cleanup()
		{
			mState.clear();
			mAuthContext.clear();
		}


		void checkServerAdress_data()
		{
			QTest::addColumn<bool>("personalization");
			QTest::addColumn<QUrl>("url");

			QTest::newRow("authentication") << false << QUrl("https://eid-server.example.de/entrypoint"_L1);
			QTest::newRow("personalization") << true << QUrl("https://eid-server.example.de/personalization"_L1);
		}


		void checkServerAdress()
		{
			QFETCH(bool, personalization);
			QFETCH(QUrl, url);

			const_cast<bool&>(mState->mPersonalization) = personalization;

			mNetworkManager->setFilename(":/paos/DIDList.xml"_L1);
			QSharedPointer<InitializeFrameworkResponse> initializeFrameworkResponse(new InitializeFrameworkResponse());
			mAuthContext->setInitializeFrameworkResponse(initializeFrameworkResponse);

			QSignalSpy spy(mNetworkManager.data(), &MockNetworkManager::fireReply);
			mAuthContext->setStateApproved();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations

			mNetworkManager->fireFinished();
			QCOMPARE(mNetworkManager->getLastRequest().url(), url);
		}


		void setReceivedMessage_data()
		{
			QTest::addColumn<PaosType>("type");
			QTest::addColumn<QString>("messageId");
			QTest::addColumn<QSharedPointer<PaosMessage>>("message");

			QSharedPointer<PaosMessage> tmp;

			tmp = QSharedPointer<StartPaosResponse>::create(QByteArray());
			QTest::newRow("startpaosResponse") << PaosType::STARTPAOS_RESPONSE << QStringLiteral("startpaosResponse") << tmp;

			tmp = QSharedPointer<InitializeFramework>::create(QByteArray());
			QTest::newRow("initializeFramework") << PaosType::INITIALIZE_FRAMEWORK << QStringLiteral("initializeFramework") << tmp;

			tmp = QSharedPointer<DIDAuthenticateEAC1>::create();
			QTest::newRow("didAuthenticateEac1") << PaosType::DID_AUTHENTICATE_EAC1 << QStringLiteral("didAuthenticateEac1") << tmp;

			tmp = QSharedPointer<DIDAuthenticateEAC2>::create();
			QTest::newRow("didAuthenticateEac2") << PaosType::DID_AUTHENTICATE_EAC2 << QStringLiteral("didAuthenticateEac2") << tmp;

			tmp = QSharedPointer<DIDAuthenticateEACAdditional>::create();
			QTest::newRow("didAuthenticateEacAdditionalInputType") << PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE << QStringLiteral("didAuthenticateEacAdditionalInputType") << tmp;

			tmp = QSharedPointer<Transmit>::create();
			QTest::newRow("transmit") << PaosType::TRANSMIT << QStringLiteral("transmit") << tmp;

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
					QCOMPARE(mAuthContext->getTransmit(), message.staticCast<Transmit>());
					QVERIFY(mAuthContext->getTransmitResponse());
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
			reply->setError(QNetworkReply::ConnectionRefusedError, "forced connection refused"_L1);
			mNetworkManager->setNextReply(reply);
			QSharedPointer<InitializeFrameworkResponse> initializeFrameworkResponse(new InitializeFrameworkResponse());
			mAuthContext->setInitializeFrameworkResponse(initializeFrameworkResponse);

			QSignalSpy spy(mState.data(), &StateGenericSendReceive::fireAbort);
			QSignalSpy spyMock(mNetworkManager.data(), &MockNetworkManager::fireReply);

			mAuthContext->setStateApproved();
			QTRY_COMPARE(spyMock.count(), 1); // clazy:exclude=qstring-allocations
			mNetworkManager->fireFinished();

			QCOMPARE(spy.count(), 1);
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Generic_Send_Receive_Network_Error);
		}


		void sendInitializeFrameworkResponse_receiveDIDAuthenticateEAC1()
		{
			mNetworkManager->setFilename(":/paos/DIDAuthenticateEAC1.xml"_L1);
			QSharedPointer<InitializeFrameworkResponse> initializeFrameworkResponse(new InitializeFrameworkResponse());
			mAuthContext->setInitializeFrameworkResponse(initializeFrameworkResponse);

			QSignalSpy spy(mState.data(), &StateSendInitializeFrameworkResponse::fireContinue);
			QSignalSpy spyMock(mNetworkManager.data(), &MockNetworkManager::fireReply);

			mAuthContext->setStateApproved();
			QTRY_COMPARE(spyMock.count(), 1); // clazy:exclude=qstring-allocations
			mNetworkManager->fireFinished();

			QCOMPARE(spy.count(), 1);
		}


		void sendInitializeFrameworkResponse_unexpected()
		{
			mNetworkManager->setFilename(":/paos/Transmit.xml"_L1);
			QSharedPointer<InitializeFrameworkResponse> initializeFrameworkResponse(new InitializeFrameworkResponse());
			mAuthContext->setInitializeFrameworkResponse(initializeFrameworkResponse);

			QSignalSpy spy(mState.data(), &StateGenericSendReceive::fireAbort);
			QSignalSpy spyMock(mNetworkManager.data(), &MockNetworkManager::fireReply);

			mAuthContext->setStateApproved();
			QTRY_COMPARE(spyMock.count(), 1); // clazy:exclude=qstring-allocations
			mNetworkManager->fireFinished();

			QCOMPARE(spy.count(), 1);
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Generic_Send_Receive_Paos_Unexpected);
		}


		void mappingToTrustedChannelError()
		{
			const QList<GlobalStatus::Code> states = QList<GlobalStatus::Code>()
					<< GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error
					<< GlobalStatus::Code::Workflow_TrustedChannel_Server_Error
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


		void logging_data()
		{
			QTest::addColumn<QList<Pair>>("attributes");
			QTest::addColumn<bool>("enabled");

			QTest::newRow("no attr") << QList<Pair>() << true;

			QTest::newRow("single attr lower-case single") << QList<Pair>({
						{QByteArray("pragma"), QByteArray("no-log")}
					}) << false;
			QTest::newRow("single attr upper-case single") << QList<Pair>({
						{QByteArray("Pragma"), QByteArray("no-log")}
					}) << false;
			QTest::newRow("single attr lower-case multi") << QList<Pair>({
						{QByteArray("pragma"), QByteArray("no-log no-cache")}
					}) << false;
			QTest::newRow("single attr upper-case multi") << QList<Pair>({
						{QByteArray("Pragma"), QByteArray("no-log no-cache")}
					}) << false;
			QTest::newRow("single attr other") << QList<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")}
					}) << true;

			QTest::newRow("multi attr lower-case single") << QList<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")},
						{QByteArray("pragma"), QByteArray("no-log")}
					}) << false;
			QTest::newRow("multi attr upper-case single") << QList<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")},
						{QByteArray("Pragma"), QByteArray("no-log")}
					}) << false;
			QTest::newRow("multi attr lower-case multi") << QList<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")},
						{QByteArray("pragma"), QByteArray("no-log no-cache")}
					}) << false;
			QTest::newRow("multi attr upper-case multi") << QList<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")},
						{QByteArray("Pragma"), QByteArray("no-log no-cache")}
					}) << false;
		}


		void logging()
		{
			QFETCH(QList<Pair>, attributes);
			QFETCH(bool, enabled);

			MockNetworkReply* reply = new MockNetworkReply(QByteArrayLiteral("TEST"));
			for (const auto& attribute : attributes)
			{
				reply->setRawHeader(attribute.first, attribute.second);
			}
			mNetworkManager->setNextReply(reply);

			QSharedPointer<InitializeFrameworkResponse> initializeFrameworkResponse(new InitializeFrameworkResponse());
			mAuthContext->setInitializeFrameworkResponse(initializeFrameworkResponse);

			QTest::ignoreMessage(QtDebugMsg, "Status Code: 200 \"OK\"");
			if (enabled)
			{
				QTest::ignoreMessage(QtDebugMsg, "Received raw data:\n TEST");
			}
			else
			{
				QTest::ignoreMessage(QtDebugMsg, "no-log was requested, skip logging of raw data");
			}

			QSignalSpy spyMock(mNetworkManager.data(), &MockNetworkManager::fireReply);

			mAuthContext->setStateApproved();
			QTRY_COMPARE(spyMock.count(), 1);     // clazy:exclude=qstring-allocations
			mNetworkManager->fireFinished();
		}


		void onReplyFinishedError_data()
		{
			QTest::addColumn<QNetworkReply::NetworkError>("networkError");
			QTest::addColumn<GlobalStatus::Code>("globalStatus");
			QTest::addColumn<FailureCode::Reason>("failureCode");
			QTest::addColumn<int>("httpCode");

			QTest::addRow("http service unavailable") << QNetworkReply::ServiceUnavailableError << GlobalStatus::Code::Workflow_TrustedChannel_ServiceUnavailable << FailureCode::Reason::Generic_Send_Receive_Service_Unavailable << 503;
			QTest::addRow("http server error") << QNetworkReply::InternalServerError << GlobalStatus::Code::Workflow_TrustedChannel_Server_Error << FailureCode::Reason::Generic_Send_Receive_Server_Error << 500;
			QTest::addRow("http not found") << QNetworkReply::ContentNotFoundError << GlobalStatus::Code::Workflow_TrustedChannel_Client_Error << FailureCode::Reason::Generic_Send_Receive_Client_Error << 404;
			QTest::addRow("http other error") << QNetworkReply::ProtocolUnknownError << GlobalStatus::Code::Workflow_TrustedChannel_Other_Network_Error << FailureCode::Reason::Generic_Send_Receive_Network_Error << 301;
		}


		void onReplyFinishedError()
		{
			QFETCH(QNetworkReply::NetworkError, networkError);
			QFETCH(GlobalStatus::Code, globalStatus);
			QFETCH(FailureCode::Reason, failureCode);
			QFETCH(int, httpCode);

			auto reply = QSharedPointer<MockNetworkReply>::create();
			reply->setAttribute(QNetworkRequest::HttpStatusCodeAttribute, QVariant(httpCode));
			reply->setError(networkError, QString());
			mState->mReply = reply;

			mState->onReplyFinished();

			QCOMPARE(mAuthContext->getStatus().getStatusCode(), globalStatus);
			QVERIFY(mAuthContext->getFailureCode().has_value());
			QCOMPARE(mAuthContext->getFailureCode().value().getReason(), failureCode);
		}


};

QTEST_GUILESS_MAIN(test_StateGenericSendReceive)
#include "test_StateGenericSendReceive.moc"
