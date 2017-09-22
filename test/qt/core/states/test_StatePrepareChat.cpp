/*!
 * \brief Unit tests for \ref StatePreVerification
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "states/StatePrepareChat.h"

#include "paos/retrieve/DidAuthenticateEac1.h"
#include "paos/retrieve/DidAuthenticateEac1Parser.h"
#include "states/StateBuilder.h"
#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;


class test_StatePrepareChat
	: public QObject
{
	Q_OBJECT
	QScopedPointer<StatePrepareChat> mState;
	QSharedPointer<AuthContext> mAuthContext;

	void verifyEffectiveChat()
	{
		// effective chat is set to required chat plus optional chat
		QSharedPointer<const CHAT> effectiveChat = mState->getContext()->getEffectiveChat();
		QSharedPointer<const CHAT> optionalChat = mState->getContext()->getOptionalChat();
		QSharedPointer<const CHAT> requiredChat = mState->getContext()->getRequiredChat();

		QSet<AccessRight> optionalPlusRequired;
		if (requiredChat)
		{
			optionalPlusRequired += requiredChat->getAccessRights();
		}
		if (optionalChat)
		{
			optionalPlusRequired += optionalChat->getAccessRights();
		}
		QCOMPARE(optionalPlusRequired, effectiveChat->getAccessRights());
	}


	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new AuthContext(nullptr));
			QSharedPointer<DIDAuthenticateEAC1> didAuthEac1(dynamic_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1_3.xml"))));
			mAuthContext->setDidAuthenticateEac1(didAuthEac1);
			mAuthContext->setTerminalCvc(didAuthEac1->getCvCertificates().at(0));
			mAuthContext->setDvCvc(didAuthEac1->getCvCertificates().at(1));

			mState.reset(StateBuilder::createState<StatePrepareChat>(mAuthContext));
			connect(this, &test_StatePrepareChat::fireStateStart, mState.data(), &AbstractState::onEntry, Qt::ConnectionType::DirectConnection);
		}


		void cleanup()
		{
			mAuthContext.clear();
			mState.reset();
		}


		void testNoOptionalChatNoRequiredChat()
		{
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mOptionalChat.reset();
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mRequiredChat.reset();

			QSignalSpy spy(mState.data(), &StatePrepareChat::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QCOMPARE(mState->getContext()->getOptionalChat()->getAccessRights(), mState->getContext()->getTerminalCvc()->getBody().getCHAT().getAccessRights());
			QVERIFY(!mState->getContext()->getRequiredChat());
			verifyEffectiveChat();
		}


		void testOptionalChatNoRequiredChat()
		{
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mRequiredChat.reset();

			QSignalSpy spy(mState.data(), &StatePrepareChat::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QCOMPARE(mState->getContext()->getOptionalChat()->getAccessRights(), mState->getContext()->mDIDAuthenticateEAC1->getOptionalChat()->getAccessRights());
			QVERIFY(!mState->getContext()->getRequiredChat());
			verifyEffectiveChat();
		}


		void testNoOptionalChatRequiredChat()
		{
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mOptionalChat.reset();

			QSignalSpy spy(mState.data(), &StatePrepareChat::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(!mState->getContext()->getOptionalChat());
			QCOMPARE(mState->getContext()->getRequiredChat()->getAccessRights(), mState->getContext()->mDIDAuthenticateEAC1->getRequiredChat()->getAccessRights());
			verifyEffectiveChat();
		}


		void testOptionalChatRequiredChat_butNoIntersection()
		{
			QSignalSpy spy(mState.data(), &StatePrepareChat::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QCOMPARE(mState->getContext()->getOptionalChat()->getAccessRights(), mState->getContext()->mDIDAuthenticateEAC1->getOptionalChat()->getAccessRights());
			QCOMPARE(mState->getContext()->getRequiredChat()->getAccessRights(), mState->getContext()->mDIDAuthenticateEAC1->getRequiredChat()->getAccessRights());
			verifyEffectiveChat();
		}


		void testOptionalChatRequiredChat_butIntersection()
		{
			QSet<governikus::AccessRight> optionalChatFromEac1WithoutDG05 = mAuthContext->mDIDAuthenticateEAC1->getOptionalChat()->getAccessRights();
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mOptionalChat->setAccessRight(AccessRight::READ_DG05); // Family name also in required chat

			QSignalSpy spy(mState.data(), &StatePrepareChat::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);


			QCOMPARE(mState->getContext()->getOptionalChat()->getAccessRights(), optionalChatFromEac1WithoutDG05);
			QCOMPARE(mState->getContext()->getRequiredChat()->getAccessRights(), mState->getContext()->mDIDAuthenticateEAC1->getRequiredChat()->getAccessRights());
			verifyEffectiveChat();
		}


		void testEncodeEffectiveChat()
		{
			QSignalSpy spy(mState.data(), &StatePrepareChat::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QVERIFY(!mState->getContext()->getEffectiveChat()->encode().isEmpty());
		}


		void testOptionalChatWithNotAllowedAccessRight()
		{
			QSet<AccessRight> accessRights = mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mOptionalChat->getAccessRights();
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mOptionalChat->setAccessRight(AccessRight::PIN_MANAGEMENT);

			QSignalSpy spy(mState.data(), &StatePrepareChat::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QCOMPARE(mState->getContext()->getOptionalChat()->getAccessRights(), accessRights);
		}


		void testRequiredChatWithNotAllowedAccessRight()
		{
			QSet<AccessRight> accessRights = mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mRequiredChat->getAccessRights();
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mRequiredChat->setAccessRight(AccessRight::PIN_MANAGEMENT);

			QSignalSpy spy(mState.data(), &StatePrepareChat::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QCOMPARE(mState->getContext()->getRequiredChat()->getAccessRights(), accessRights);
		}


		void testOptionalChatWithNotDisplayedAccessRight()
		{
			mAuthContext->mTerminalCvc->mBody->mChat->setAccessRight(AccessRight::READ_DG18);
			QSet<AccessRight> accessRights = mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mOptionalChat->getAccessRights();
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mOptionalChat->setAccessRight(AccessRight::READ_DG18);

			QSignalSpy spy(mState.data(), &StatePrepareChat::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QCOMPARE(mState->getContext()->getOptionalChat()->getAccessRights(), accessRights);
		}


		void testRequiredChatWithNotDisplayedAccessRight()
		{
			mAuthContext->mTerminalCvc->mBody->mChat->setAccessRight(AccessRight::READ_DG18);
			QSet<AccessRight> accessRights = mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mRequiredChat->getAccessRights();
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mRequiredChat->setAccessRight(AccessRight::READ_DG18);

			QSignalSpy spy(mState.data(), &StatePrepareChat::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QCOMPARE(mState->getContext()->getRequiredChat()->getAccessRights(), accessRights);
		}


};

QTEST_GUILESS_MAIN(test_StatePrepareChat)
#include "test_StatePrepareChat.moc"
