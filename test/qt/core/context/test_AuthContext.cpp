/*!
 * test_AuthContext.cpp
 *
 * \brief Test for the chat administration in authentication context.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "context/AuthContext.h"
#include "testMacros.h"

#include <QSharedPointer>
#include <QSignalSpy>
#include <QtTest>


using namespace governikus;


class test_AuthContext
	: public QObject
{
	Q_OBJECT

	private:
		QScopedPointer<QSignalSpy> mEffectiveSignal;
		QScopedPointer<QSignalSpy> mRequiredSignal;
		QScopedPointer<QSignalSpy> mOptionalSignal;
		QScopedPointer<AuthContext> mAuthContext;

		QSharedPointer<CHAT> getChat(const std::initializer_list<AccessRight>& pList)
		{
			auto chat = newObject<CHAT>();
			chat->setAccessRights(QSet<AccessRight>(pList));
			return chat;
		}


		void checkAndResetSignals(int pRequired, int pOptional, int pEffective)
		{
			QCOMPARE(mRequiredSignal->count(), pRequired);
			QCOMPARE(mOptionalSignal->count(), pOptional);
			QCOMPARE(mEffectiveSignal->count(), pEffective);

			mRequiredSignal->clear();
			mOptionalSignal->clear();
			mEffectiveSignal->clear();
		}


	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new AuthContext(nullptr));
			mEffectiveSignal.reset(new QSignalSpy(mAuthContext.data(), &AuthContext::fireEffectiveChatChanged));
			mRequiredSignal.reset(new QSignalSpy(mAuthContext.data(), &AuthContext::fireRequiredChatChanged));
			mOptionalSignal.reset(new QSignalSpy(mAuthContext.data(), &AuthContext::fireOptionalChatChanged));
		}


		void cleanup()
		{
			mEffectiveSignal.reset();
			mRequiredSignal.reset();
			mOptionalSignal.reset();
			mAuthContext.reset();
		}


		void emptyOnCreation()
		{
			QVERIFY(!mAuthContext->getEffectiveChat());
			QVERIFY(!mAuthContext->getRequiredChat());
			QVERIFY(!mAuthContext->getOptionalChat());
		}


		void setEffectiveWithoutRequiredAndOptional()
		{
			QVERIFY(mAuthContext->setEffectiveChat(getChat({})));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 0);
			checkAndResetSignals(0, 0, 1);

			QVERIFY(!mAuthContext->setEffectiveChat(getChat({AccessRight::AGE_VERIFICATION})));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 0);
			checkAndResetSignals(0, 0, 1);
		}


		void requiredChat()
		{
			mAuthContext->setRequiredChat(getChat({AccessRight::AGE_VERIFICATION}));
			QVERIFY(mAuthContext->getRequiredChat());
			QVERIFY(!mAuthContext->getEffectiveChat());
			checkAndResetSignals(1, 0, 0);

			QVERIFY(!mAuthContext->setEffectiveChat(getChat({})));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 0, 1);

			QVERIFY(mAuthContext->setEffectiveChat(getChat({AccessRight::AGE_VERIFICATION})));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 0, 1);

			mAuthContext->setRequiredChat(getChat({}));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 0);
			checkAndResetSignals(1, 0, 1);

			mAuthContext->setRequiredChat(getChat({AccessRight::AGE_VERIFICATION}));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(1, 0, 1);

			mAuthContext->setOptionalChat(getChat({AccessRight::AGE_VERIFICATION}));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 1, 0);

			QVERIFY(mAuthContext->setEffectiveChat(getChat({AccessRight::AGE_VERIFICATION})));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 0, 1);

			mAuthContext->setRequiredChat(getChat({}));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(1, 0, 0);
		}


		void optionalChat()
		{
			mAuthContext->setOptionalChat(getChat({AccessRight::AGE_VERIFICATION}));
			QVERIFY(mAuthContext->getOptionalChat());
			QVERIFY(!mAuthContext->getEffectiveChat());
			checkAndResetSignals(0, 1, 0);

			QVERIFY(mAuthContext->setEffectiveChat(getChat({})));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 0);
			checkAndResetSignals(0, 0, 1);

			QVERIFY(mAuthContext->setEffectiveChat(getChat({AccessRight::AGE_VERIFICATION})));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 0, 1);

			mAuthContext->setOptionalChat(getChat({}));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 0);
			checkAndResetSignals(0, 1, 1);

			mAuthContext->setRequiredChat(getChat({AccessRight::AGE_VERIFICATION}));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(1, 0, 1);

			mAuthContext->setOptionalChat(getChat({AccessRight::AGE_VERIFICATION}));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 1, 0);

			QVERIFY(mAuthContext->setEffectiveChat(getChat({AccessRight::AGE_VERIFICATION})));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 0, 1);

			mAuthContext->setOptionalChat(getChat({}));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 1, 0);
		}


		void effectiveChat()
		{
			mAuthContext->setRequiredChat(getChat({AccessRight::AGE_VERIFICATION}));
			checkAndResetSignals(1, 0, 0);

			mAuthContext->setOptionalChat(getChat({AccessRight::INSTALL_QUAL_CERT}));
			checkAndResetSignals(0, 1, 0);

			QVERIFY(!mAuthContext->setEffectiveChat(getChat({})));
			checkAndResetSignals(0, 0, 1);

			QVERIFY(!mAuthContext->addEffectiveAccessRight(AccessRight::PRIVILEGED_TERMINAL));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 0, 0);

			QVERIFY(mAuthContext->removeEffectiveAccessRight(AccessRight::PRIVILEGED_TERMINAL));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 0, 0);

			QVERIFY(!mAuthContext->removeEffectiveAccessRight(AccessRight::AGE_VERIFICATION));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 0, 0);

			QVERIFY(mAuthContext->addEffectiveAccessRight(AccessRight::INSTALL_QUAL_CERT));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 2);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::INSTALL_QUAL_CERT));
			checkAndResetSignals(0, 0, 1);

			QVERIFY(mAuthContext->addEffectiveAccessRight(AccessRight::INSTALL_QUAL_CERT));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 2);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::INSTALL_QUAL_CERT));
			checkAndResetSignals(0, 0, 0);

			QVERIFY(mAuthContext->removeEffectiveAccessRight(AccessRight::INSTALL_QUAL_CERT));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 0, 1);

			QVERIFY(!mAuthContext->setEffectiveAccessRights({}));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 0, 0);

			QVERIFY(mAuthContext->setEffectiveAccessRights({AccessRight::AGE_VERIFICATION, AccessRight::INSTALL_QUAL_CERT}));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 2);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::INSTALL_QUAL_CERT));
			checkAndResetSignals(0, 0, 1);

			QVERIFY(!mAuthContext->setEffectiveAccessRights({AccessRight::AGE_VERIFICATION, AccessRight::PRIVILEGED_TERMINAL}));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 1);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			checkAndResetSignals(0, 0, 1);

			QVERIFY(!mAuthContext->setEffectiveAccessRights({AccessRight::INSTALL_QUAL_CERT, AccessRight::PRIVILEGED_TERMINAL}));
			QCOMPARE(mAuthContext->getEffectiveChat()->getAccessRights().size(), 2);
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::AGE_VERIFICATION));
			QVERIFY(mAuthContext->getEffectiveChat()->getAccessRights().contains(AccessRight::INSTALL_QUAL_CERT));
			checkAndResetSignals(0, 0, 1);
		}


};

QTEST_GUILESS_MAIN(test_AuthContext)
#include "test_AuthContext.moc"
