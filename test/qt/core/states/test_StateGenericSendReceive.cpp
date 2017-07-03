/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "TestAuthContext.h"
#include "controller/AuthController.h"
#include "paos/invoke/InitializeFrameworkResponse.h"
#include "paos/retrieve/InitializeFramework.h"
#include "states/StateBuilder.h"
#include "states/StateGenericSendReceive.h"

#include "MockNetworkManager.h"

#include <QFile>
#include <QString>
#include <QtCore/QtCore>
#include <QtTest/QtTest>

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

			QFile tcTokenFile(":/tctoken/ok.xml");
			QVERIFY(tcTokenFile.open(QIODevice::ReadOnly));
			QSharedPointer<TcToken> tcToken(new TcToken(tcTokenFile.readAll()));
			mAuthContext->setTcToken(tcToken);
			mNetworkManager = new MockNetworkManager();
			mAuthContext->setNetworkManager(mNetworkManager.data());

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

			QSignalSpy spy(mState.data(), &StateGenericSendReceive::fireError);

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

			QSignalSpy spy(mState.data(), &StateGenericSendReceive::fireSuccess);

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

			QSignalSpy spy(mState.data(), &StateGenericSendReceive::fireError);

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
