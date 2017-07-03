/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "states/StateCheckRefreshAddress.h"

#include "context/AuthContext.h"

#include <QThread>
#include <QtCore>
#include <QtTest>

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
			mState.reset(new StateCheckRefreshAddress(mAuthContext));
			mState->setStateName("StateCheckRefreshAddress");
			connect(this, &test_StateCheckRefreshAddress::fireStateStart, mState.data(), &AbstractState::onEntry, Qt::ConnectionType::DirectConnection);
		}


		void cleanup()
		{
			mState.reset();
			mAuthContext.reset();
		}


		void sendGetRequestWithoutAnyAdditionalParameters()
		{
			QUrl redirectUrl("http://localhost:12345/test_StateCheckRefreshAddress/");
			Result result = Result::createCommunicationError("Fehlertext");
			mAuthContext->setResult(result);
			mState->mUrl = redirectUrl;

			mState->sendGetRequest();

			mState->mReply->abort();
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
