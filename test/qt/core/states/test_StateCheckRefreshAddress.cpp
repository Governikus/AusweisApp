/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "states/StateCheckRefreshAddress.h"

#include "context/AuthContext.h"
#include "MockNetworkManager.h"
#include "states/StateBuilder.h"

#include <QtCore>
#include <QtTest>
#include <QThread>

using namespace governikus;


class test_StateCheckRefreshAddress
	: public QObject
{
	Q_OBJECT
	QScopedPointer<StateCheckRefreshAddress> mState;
	QSharedPointer<AuthContext> mAuthContext;

	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new AuthContext(nullptr));
			mAuthContext->setNetworkManager(new MockNetworkManager);
			mState.reset(StateBuilder::createState<StateCheckRefreshAddress>(mAuthContext));
			connect(this, &test_StateCheckRefreshAddress::fireStateStart, mState.data(), &AbstractState::onEntry, Qt::ConnectionType::DirectConnection);
		}


		void cleanup()
		{
			mState.reset();
			mAuthContext.reset();
		}


		void mappingToCommunicationError()
		{
			const QVector<GlobalStatus::Code> states = QVector<GlobalStatus::Code>()
					<< GlobalStatus::Code::Network_Ssl_Establishment_Error
					<< GlobalStatus::Code::Network_Ssl_Establishment_Error
					<< GlobalStatus::Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length
					<< GlobalStatus::Code::Workflow_Network_Ssl_Certificate_Unsupported_Algorithm_Or_Length
					<< GlobalStatus::Code::Workflow_Nerwork_Ssl_Hash_Not_In_Certificate_Description
					<< GlobalStatus::Code::Workflow_Network_Empty_Redirect_Url
					<< GlobalStatus::Code::Workflow_Network_Expected_Redirect
					<< GlobalStatus::Code::Workflow_Network_Invalid_Scheme
					<< GlobalStatus::Code::Workflow_Network_Malformed_Redirect_Url
					<< GlobalStatus::Code::Network_TimeOut
					<< GlobalStatus::Code::Network_Proxy_Error;

			for (const GlobalStatus::Code state : states)
			{
				const Result& result = GlobalStatus(state);
				QCOMPARE(result.getMinor(), GlobalStatus::Code::Paos_Error_AL_Communication_Error);
			}
		}


		void sendGetRequestWithoutAnyAdditionalParameters()
		{
			QUrl redirectUrl("http://localhost:12345/test_StateCheckRefreshAddress/");
			mAuthContext->setStatus(GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
			mState->mUrl = redirectUrl;

			mState->sendGetRequest();
			QCOMPARE(mState->mReply->request().url(), redirectUrl);
		}


		void abortIfNotTcToken()
		{
			QSignalSpy spy(mState.data(), &StateCheckRefreshAddress::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(mAuthContext->getRefreshUrl().isEmpty());
		}


		void abortIfRefreshAddressIsNotHttps()
		{
			QSharedPointer<TcToken> tcToken(new TcToken("<?xml version=\"1.0\"?>"
														"<TCTokenType>"
														"  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
														"  <SessionIdentifier>1A2BB129</SessionIdentifier>"
														"  <RefreshAddress>http://service.example.de/loggedin?7eb39f62</RefreshAddress>"
														"  <Binding> urn:liberty:paos:2006-08 </Binding>"
														"  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
														"  <PathSecurity-Parameters>"
														"    <PSK> 4BC1A0B5 </PSK>"
														"  </PathSecurity-Parameters>"
														"</TCTokenType>"));
			mAuthContext->setTcToken(tcToken);

			QSignalSpy spy(mState.data(), &StateCheckRefreshAddress::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(mAuthContext->getRefreshUrl().isEmpty());
		}


};

QTEST_GUILESS_MAIN(test_StateCheckRefreshAddress)
#include "test_StateCheckRefreshAddress.moc"
