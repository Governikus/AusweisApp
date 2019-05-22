/*!
 * \brief Unit tests for \ref ChatModel
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ChatModel.h"

#include "context/SelfAuthContext.h"

#include "MockActivationContext.h"

#include <QDebug>
#include <QtTest>


using namespace governikus;


class test_ChatModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_ResetContext()
		{
			ChatModel model;
			QSharedPointer<MockActivationContext> activationContext(new MockActivationContext());
			QSharedPointer<AuthContext> authContext(new AuthContext(activationContext));
			QSharedPointer<SelfAuthContext> selfAuthContext(new SelfAuthContext());

			model.resetContext(nullptr);
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG05));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG13));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG04));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG07));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG08));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG09));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG17));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG01));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG10));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG06));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG02));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG19));
			QCOMPARE(model.mSelectedRights, model.mAllRights.toSet());

			model.resetContext(selfAuthContext);
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG05));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG13));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG04));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG07));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG08));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG09));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG17));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG01));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG10));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG06));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG02));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG19));
			QCOMPARE(model.mSelectedRights, model.mAllRights.toSet());

			model.resetContext(authContext);
			QVERIFY(model.mAllRights.isEmpty());
			QVERIFY(model.mOptionalRights.isEmpty());
			QVERIFY(model.mSelectedRights.isEmpty());
		}


		void test_OnAuthenticationDataChanged()
		{
			ChatModel model;
			QSharedPointer<MockActivationContext> activationContext(new MockActivationContext());
			QSharedPointer<AuthContext> authContext(new AuthContext(activationContext));
			model.mAuthContext = authContext;

			model.onAuthenticationDataChanged();
			QVERIFY(model.mAllRights.isEmpty());
			QVERIFY(model.mOptionalRights.isEmpty());
			QVERIFY(model.mSelectedRights.isEmpty());

			authContext->mRequiredAccessRights += AccessRight::READ_DG01;
			authContext->mRequiredAccessRights += AccessRight::READ_DG04;

			model.onAuthenticationDataChanged();
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG01));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG04));
			QVERIFY(model.mSelectedRights.contains(AccessRight::READ_DG01));
			QVERIFY(model.mSelectedRights.contains(AccessRight::READ_DG04));
			QVERIFY(model.mOptionalRights.isEmpty());

			authContext->mOptionalAccessRights += AccessRight::READ_DG10;
			authContext->mOptionalAccessRights += AccessRight::READ_DG17;

			model.onAuthenticationDataChanged();
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG01));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG04));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG10));
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG17));
			QVERIFY(model.mSelectedRights.contains(AccessRight::READ_DG01));
			QVERIFY(model.mSelectedRights.contains(AccessRight::READ_DG04));
			QVERIFY(model.mSelectedRights.contains(AccessRight::READ_DG10));
			QVERIFY(model.mSelectedRights.contains(AccessRight::READ_DG17));
			QVERIFY(model.mSelectedRights.contains(AccessRight::READ_DG10));
			QVERIFY(model.mSelectedRights.contains(AccessRight::READ_DG17));
		}


		void test_SetOrderedAllRights()
		{
			ChatModel model;
			QSet<AccessRight> rights;
			rights.insert(AccessRight::READ_DG01);
			rights.insert(AccessRight::INSTALL_QUAL_CERT);
			rights.insert(AccessRight::CAN_ALLOWED);
			model.setOrderedAllRights(rights);
			QVERIFY(model.mAllRights.contains(AccessRight::READ_DG01));
			QVERIFY(model.mAllRights.contains(AccessRight::INSTALL_QUAL_CERT));
			QVERIFY(!model.mAllRights.contains(AccessRight::CAN_ALLOWED));
		}


		void test_TransferAccessRights()
		{
			ChatModel model;
			QSharedPointer<MockActivationContext> activationContext(new MockActivationContext());
			QSharedPointer<AuthContext> authContext(new AuthContext(activationContext));
			authContext->mOptionalAccessRights += model.mSelectedRights += AccessRight::INSTALL_QUAL_CERT;
			authContext->mOptionalAccessRights += model.mSelectedRights += AccessRight::READ_DG01;
			QVERIFY(!authContext->getEffectiveAccessRights().contains(AccessRight::INSTALL_QUAL_CERT));
			QVERIFY(!authContext->getEffectiveAccessRights().contains(AccessRight::READ_DG01));

			model.mAuthContext = authContext;
			model.mSelectedRights += AccessRight::INSTALL_QUAL_CERT;
			model.mSelectedRights += AccessRight::READ_DG01;

			model.transferAccessRights();
			QVERIFY(authContext->getEffectiveAccessRights().contains(AccessRight::INSTALL_QUAL_CERT));
			QVERIFY(authContext->getEffectiveAccessRights().contains(AccessRight::READ_DG01));
		}


};

QTEST_GUILESS_MAIN(test_ChatModel)
#include "test_ChatModel.moc"
