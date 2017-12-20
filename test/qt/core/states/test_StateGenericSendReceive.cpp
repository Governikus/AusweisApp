/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "controller/AuthController.h"
#include "Env.h"
#include "paos/invoke/InitializeFrameworkResponse.h"
#include "paos/retrieve/InitializeFramework.h"
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
				const Result& result = Result(GlobalStatus(state));
				QCOMPARE(result.getMinor(), GlobalStatus::Code::Paos_Error_DP_Trusted_Channel_Establishment_Failed);
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
