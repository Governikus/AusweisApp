/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "Env.h"
#include "states/StateBuilder.h"
#include "states/StateGenericSendReceive.h"

#include "MockNetworkManager.h"
#include "TestAuthContext.h"
#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;

class test_StateGenericSendReceive
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<StateSendInitializeFrameworkResponse> mState;
		QSharedPointer<AuthContext> mAuthContext;
		QPointer<MockNetworkManager> mNetworkManager;

	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1.xml"));

			QSharedPointer<TcToken> tcToken(new TcToken(TestFileHelper::readFile(":/tctoken/ok.xml")));
			mAuthContext->setTcToken(tcToken);
			mNetworkManager = new MockNetworkManager();
			Env::set(NetworkManager::staticMetaObject, mNetworkManager.data());

			mState.reset(StateBuilder::createState<StateSendInitializeFrameworkResponse>(mAuthContext));
			mState->setStateName("StateSendInitializeFrameworkResponse");
			connect(this, &test_StateGenericSendReceive::fireStateStart, mState.data(), &AbstractState::onEntry, Qt::ConnectionType::DirectConnection);
		}


		void setReceivedMessage_data()
		{
			QTest::addColumn<PaosType>("type");
			QTest::addColumn<QString>("messageId");
			QTest::addColumn<int>("testCase");

			QTest::newRow("startpaosResponse") << PaosType::STARTPAOS_RESPONSE << QString("startpaosResponse") << 0;
			QTest::newRow("initializeFramework") << PaosType::INITIALIZE_FRAMEWORK << QString("initializeFramework") << 1;
			QTest::newRow("didList") << PaosType::DID_LIST << QString("didList") << 2;
			QTest::newRow("didAuthenticateEac1") << PaosType::DID_AUTHENTICATE_EAC1 << QString("didAuthenticateEac1") << 3;
			QTest::newRow("didAuthenticateEac2") << PaosType::DID_AUTHENTICATE_EAC2 << QString("didAuthenticateEac2") << 4;
			QTest::newRow("didAuthenticateEacAdditionalInputType") << PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE << QString("didAuthenticateEacAdditionalInputType") << 5;
			QTest::newRow("transmit") << PaosType::TRANSMIT << QString("transmit") << 6;
			QTest::newRow("disconnect") << PaosType::DISCONNECT << QString("disconnect") << 7;
			QTest::newRow("default") << PaosType::UNKNOWN << QString("default") << 8;
		}


		void setReceivedMessage()
		{
			QFETCH(PaosType, type);
			QFETCH(QString, messageId);
			QFETCH(int, testCase);

			const QSharedPointer<PaosMessage> message(new PaosMessage(type));
			message->setMessageId(messageId);

			if (testCase == 8)
			{
				QTest::ignoreMessage(QtWarningMsg, "Unknown received message type: 0");
			}

			mState->setReceivedMessage(message);
			QCOMPARE(mAuthContext->getReceivedMessageId(), messageId);

			switch (testCase)
			{
				case 0:
					QCOMPARE(mAuthContext->getStartPaosResponse(), message.staticCast<StartPaosResponse>());
					break;

				case 1:
					QCOMPARE(mAuthContext->getInitializeFramework(), message.staticCast<InitializeFramework>());
					QVERIFY(mAuthContext->getInitializeFrameworkResponse());
					break;

				case 2:
					QCOMPARE(mAuthContext->getDidList(), message.staticCast<DIDList>());
					QVERIFY(mAuthContext->getDidListResponse());
					break;

				case 3:
					QCOMPARE(mAuthContext->getDidAuthenticateEac1(), message.staticCast<DIDAuthenticateEAC1>());
					QVERIFY(mAuthContext->getDidAuthenticateResponseEac1());
					break;

				case 4:
					QCOMPARE(mAuthContext->getDidAuthenticateEac2(), message.staticCast<DIDAuthenticateEAC2>());
					QVERIFY(mAuthContext->getDidAuthenticateResponseEac2());
					break;

				case 5:
					QCOMPARE(mAuthContext->getDidAuthenticateEacAdditional(), message.staticCast<DIDAuthenticateEACAdditional>());
					break;

				case 6:
					QVERIFY(mAuthContext->getTransmits().contains(message.staticCast<Transmit>()));
					QVERIFY(!mAuthContext->getTransmitResponses().isEmpty());
					break;

				case 7:
					QCOMPARE(mAuthContext->getDisconnect(), message.staticCast<Disconnect>());
					QVERIFY(mAuthContext->getDisconnectResponse());
					break;
			}
		}


		void onPreSharedKeyAuthenticationRequired()
		{
			const QByteArray data("data");
			const QByteArray psk("psk");
			const QByteArray sessionIdentifier("session");
			const QSharedPointer<TcToken> token(new TcToken(data));
			token->mPsk = psk;
			token->mSessionIdentifier = sessionIdentifier;
			mAuthContext->setTcToken(token);
			auto* authenticator = new QSslPreSharedKeyAuthenticator();

			QTest::ignoreMessage(QtDebugMsg, "pre-shared key authentication requested: \"\"");
			mState->onPreSharedKeyAuthenticationRequired(authenticator);
			QCOMPARE(authenticator->identity(), sessionIdentifier);
			QCOMPARE(authenticator->preSharedKey(), QByteArray::fromHex(psk));
		}


		void connectionError()
		{
			MockNetworkReply reply;
			reply.setNetworkError(QNetworkReply::ConnectionRefusedError, "forced connection refused");
			mNetworkManager->setNextReply(&reply);
			QSharedPointer<InitializeFrameworkResponse> initializeFrameworkResponse(new InitializeFrameworkResponse());
			mAuthContext->setInitializeFrameworkResponse(initializeFrameworkResponse);

			QSignalSpy spy(mState.data(), &StateGenericSendReceive::fireAbort);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();
			mNetworkManager->fireFinished();

			QCOMPARE(spy.count(), 1);
		}


		void sendInitializeFrameworkResponse_receiveDIDList()
		{
			mNetworkManager->setFilename(":/paos/DIDList.xml");
			QSharedPointer<InitializeFrameworkResponse> initializeFrameworkResponse(new InitializeFrameworkResponse());
			mAuthContext->setInitializeFrameworkResponse(initializeFrameworkResponse);

			QSignalSpy spy(mState.data(), &StateGenericSendReceive::fireContinue);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();
			mNetworkManager->fireFinished();

			QCOMPARE(spy.count(), 1);
		}


		void sendInitializeFrameworkResponse_receiveDIDAuthenticateEAC1()
		{
			mNetworkManager->setFilename(":/paos/DIDAuthenticateEAC1.xml");
			QSharedPointer<InitializeFrameworkResponse> initializeFrameworkResponse(new InitializeFrameworkResponse());
			mAuthContext->setInitializeFrameworkResponse(initializeFrameworkResponse);

			QSignalSpy spy(mState.data(), &StateSendInitializeFrameworkResponse::fireReceivedExtractCvcsFromEac1InputType);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();
			mNetworkManager->fireFinished();

			QCOMPARE(spy.count(), 1);
		}


		void sendInitializeFrameworkResponse_unexpected()
		{
			mNetworkManager->setFilename(":/paos/Transmit.xml");
			QSharedPointer<InitializeFrameworkResponse> initializeFrameworkResponse(new InitializeFrameworkResponse());
			mAuthContext->setInitializeFrameworkResponse(initializeFrameworkResponse);

			QSignalSpy spy(mState.data(), &StateGenericSendReceive::fireAbort);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();
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
					<< GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Establishment_Error
					<< GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error
					<< GlobalStatus::Code::Workflow_TrustedChannel_Other_Network_Error;

			for (const GlobalStatus::Code state : states)
			{
				const ECardApiResult& result = ECardApiResult(GlobalStatus(state));
				QCOMPARE(result.getMinor(), ECardApiResult::Minor::DP_Trusted_Channel_Establishment_Failed);
			}
		}


		void cleanup()
		{
			mAuthContext.reset();
			mNetworkManager.clear();
		}


};

QTEST_GUILESS_MAIN(test_StateGenericSendReceive)
#include "test_StateGenericSendReceive.moc"
