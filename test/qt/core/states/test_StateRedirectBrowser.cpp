/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "states/StateRedirectBrowser.h"

#include "MockActivationContext.h"
#include "TestFileHelper.h"
#include "states/StateBuilder.h"

#include <QThread>
#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_StateRedirectBrowser
	: public QObject
{
	Q_OBJECT
	QScopedPointer<StateRedirectBrowser> mState;
	QSharedPointer<AuthContext> mAuthContext;

	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new AuthContext(nullptr));
			mState.reset(StateBuilder::createState<StateRedirectBrowser>(mAuthContext));
			connect(this, &test_StateRedirectBrowser::fireStateStart, mState.data(), &AbstractState::onEntry, Qt::ConnectionType::DirectConnection);
		}


		void cleanup()
		{
			mAuthContext.clear();
			mState.reset();
		}


		void tcTokenNotFound_sendErrorPageSuccess()
		{
			mAuthContext->setTcTokenNotFound(true);
			mAuthContext->mActivationContext.reset(new MockActivationContext(true, true, true, true));
			QSignalSpy spy(mState.data(), &StateRedirectBrowser::fireSuccess);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(qobject_cast<MockActivationContext*>(mAuthContext->getActivationContext())->isSendErroPageCalled());
		}


		void tcTokenNotFound_sendErrorPageFailure()
		{
			mAuthContext->setTcTokenNotFound(true);
			mAuthContext->mActivationContext.reset(new MockActivationContext(true, true, false, true, "send error"));
			QSignalSpy spy(mState.data(), &StateRedirectBrowser::fireError);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(qobject_cast<MockActivationContext*>(mAuthContext->getActivationContext())->isSendErroPageCalled());
		}


		void noRefreshUrl_noTcToken_sendErrorPageErrorSuccess()
		{
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->mActivationContext.reset(new MockActivationContext(true, true, true, true));
			QSignalSpy spy(mState.data(), &StateRedirectBrowser::fireSuccess);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(qobject_cast<MockActivationContext*>(mAuthContext->getActivationContext())->isSendErroPageCalled());
		}


		void noRefreshUrl_noTcToken_sendErrorPageErrorFailure()
		{
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->mActivationContext.reset(new MockActivationContext(true, true, false, true));
			QSignalSpy spy(mState.data(), &StateRedirectBrowser::fireError);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(qobject_cast<MockActivationContext*>(mAuthContext->getActivationContext())->isSendErroPageCalled());
		}


		void noRefreshUrl_tcTokenWithoutCommunicationErrorAddress_sendErrorPageErrorSuccess()
		{
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->setTcToken(QSharedPointer<TcToken>(new TcToken(TestFileHelper::readFile(":/tctoken/withoutCommunicationErrorAddress.xml"))));
			mAuthContext->mActivationContext.reset(new MockActivationContext(true, true, true, true));
			QSignalSpy spy(mState.data(), &StateRedirectBrowser::fireSuccess);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(qobject_cast<MockActivationContext*>(mAuthContext->getActivationContext())->isSendErroPageCalled());
		}


		void noRefreshUrl_tcTokenWithoutCommunicationErrorAddress_sendErrorPageErrorFailure()
		{
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->setTcToken(QSharedPointer<TcToken>(new TcToken(TestFileHelper::readFile(":/tctoken/withoutCommunicationErrorAddress.xml"))));
			mAuthContext->mActivationContext.reset(new MockActivationContext(true, true, false, true));
			QSignalSpy spy(mState.data(), &StateRedirectBrowser::fireError);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(qobject_cast<MockActivationContext*>(mAuthContext->getActivationContext())->isSendErroPageCalled());
		}


		void noRefreshUrl_sendRedirectSuccess()
		{
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->setTcToken(QSharedPointer<TcToken>(new TcToken(TestFileHelper::readFile(":/tctoken/ok.xml"))));
			mAuthContext->mActivationContext.reset(new MockActivationContext(true, true, true, true));
			QSignalSpy spy(mState.data(), &StateRedirectBrowser::fireSuccess);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(qobject_cast<MockActivationContext*>(mAuthContext->getActivationContext())->isSendRedirectCalled());
		}


		void noRefreshUrl_sendRedirectFailure()
		{
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->setTcToken(QSharedPointer<TcToken>(new TcToken(TestFileHelper::readFile(":/tctoken/ok.xml"))));
			mAuthContext->mActivationContext.reset(new MockActivationContext(true, true, true, false));
			QSignalSpy spy(mState.data(), &StateRedirectBrowser::fireError);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(qobject_cast<MockActivationContext*>(mAuthContext->getActivationContext())->isSendRedirectCalled());
		}


		void refreshUrl_sendRedirectSuccess()
		{
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->setTcToken(QSharedPointer<TcToken>(new TcToken(TestFileHelper::readFile(":/tctoken/ok.xml"))));
			mAuthContext->setRefreshUrl(QUrl("https://bla.de"));
			mAuthContext->mActivationContext.reset(new MockActivationContext(true, true, true, true));
			QSignalSpy spy(mState.data(), &StateRedirectBrowser::fireSuccess);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(qobject_cast<MockActivationContext*>(mAuthContext->getActivationContext())->isSendRedirectCalled());
		}


		void refreshUrl_sendRedirectFailure()
		{
			mAuthContext->setTcTokenNotFound(false);
			mAuthContext->setTcToken(QSharedPointer<TcToken>(new TcToken(TestFileHelper::readFile(":/tctoken/ok.xml"))));
			mAuthContext->setRefreshUrl(QUrl("https://bla.de"));
			mAuthContext->mActivationContext.reset(new MockActivationContext(true, true, true, false));
			QSignalSpy spy(mState.data(), &StateRedirectBrowser::fireError);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(qobject_cast<MockActivationContext*>(mAuthContext->getActivationContext())->isSendRedirectCalled());
		}


};

QTEST_GUILESS_MAIN(test_StateRedirectBrowser)
#include "test_StateRedirectBrowser.moc"
