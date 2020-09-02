/*!
 * \brief Unit tests for \ref ChatModel
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ChatModel.h"

#include "context/SelfAuthContext.h"

#include "paos/retrieve/DidAuthenticateEac1Parser.h"

#include "MockActivationContext.h"
#include "TestFileHelper.h"

#include <QDebug>
#include <QtTest>


using namespace governikus;


class test_ChatModel
	: public QObject
{
	Q_OBJECT
	QSharedPointer<ChatModel> mModel;
	QSharedPointer<MockActivationContext> mActContext;
	QSharedPointer<AuthContext> mAuthContext;

	private Q_SLOTS:
		void init()
		{
			mModel.reset(new ChatModel());
			mActContext.reset(new MockActivationContext());
			mAuthContext.reset(new AuthContext(mActContext));
		}


		void cleanup()
		{
			mModel.clear();
			mActContext.clear();
			mAuthContext.clear();
		}


		void test_ResetContext()
		{
			QVERIFY(mModel->mAllRights.isEmpty());
			QVERIFY(mModel->mOptionalRights.isEmpty());
			QVERIFY(mModel->mSelectedRights.isEmpty());
			QSharedPointer<SelfAuthContext> selfAuthContext(new SelfAuthContext());

			mModel->mAllRights = {AccessRight::READ_DG01, AccessRight::READ_DG02};
			mModel->mOptionalRights = {AccessRight::READ_DG02};
			mModel->mSelectedRights = {AccessRight::READ_DG01};
			mModel->resetContext(nullptr);
			QVERIFY(mModel->mAllRights.isEmpty());
			QVERIFY(mModel->mOptionalRights.isEmpty());
			QVERIFY(mModel->mSelectedRights.isEmpty());

			mModel->mAllRights = {AccessRight::READ_DG01, AccessRight::READ_DG02};
			mModel->mOptionalRights = {AccessRight::READ_DG02};
			mModel->mSelectedRights = {AccessRight::READ_DG01};
			mModel->resetContext(selfAuthContext);
			QVERIFY(mModel->mAllRights.isEmpty());
			QVERIFY(mModel->mOptionalRights.isEmpty());
			QVERIFY(mModel->mSelectedRights.isEmpty());

			mModel->mAllRights = {AccessRight::READ_DG01, AccessRight::READ_DG02};
			mModel->mOptionalRights = {AccessRight::READ_DG02};
			mModel->mSelectedRights = {AccessRight::READ_DG01};
			mModel->resetContext(mAuthContext);
			QVERIFY(mModel->mAllRights.isEmpty());
			QVERIFY(mModel->mOptionalRights.isEmpty());
			QVERIFY(mModel->mSelectedRights.isEmpty());
		}


		void test_OnAuthenticationDataChanged()
		{
			mModel->resetContext(mAuthContext);

			Q_EMIT mAuthContext->fireAuthenticationDataChanged();
			QVERIFY(mModel->mAllRights.isEmpty());
			QVERIFY(mModel->mOptionalRights.isEmpty());
			QVERIFY(mModel->mSelectedRights.isEmpty());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
			auto uniqueRights = QSet<AccessRight>(mModel->mAllRights.constBegin(), mModel->mAllRights.constEnd());
#else
			auto uniqueRights = mModel->mAllRights.toSet();
#endif
			QCOMPARE(mModel->mSelectedRights, uniqueRights);

			mAuthContext->mRequiredAccessRights += AccessRight::READ_DG01;
			mAuthContext->mRequiredAccessRights += AccessRight::READ_DG04;

			Q_EMIT mAuthContext->fireAuthenticationDataChanged();
			QVERIFY(mModel->mAllRights.contains(AccessRight::READ_DG01));
			QVERIFY(mModel->mAllRights.contains(AccessRight::READ_DG04));
			QVERIFY(mModel->mSelectedRights.contains(AccessRight::READ_DG01));
			QVERIFY(mModel->mSelectedRights.contains(AccessRight::READ_DG04));
			QVERIFY(mModel->mOptionalRights.isEmpty());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
			uniqueRights = QSet<AccessRight>(mModel->mAllRights.constBegin(), mModel->mAllRights.constEnd());
#else
			uniqueRights = mModel->mAllRights.toSet();
#endif
			QCOMPARE(mModel->mSelectedRights, uniqueRights);

			mAuthContext->mOptionalAccessRights += AccessRight::READ_DG10;
			mAuthContext->mOptionalAccessRights += AccessRight::READ_DG17;

			Q_EMIT mAuthContext->fireAuthenticationDataChanged();
			QVERIFY(mModel->mAllRights.contains(AccessRight::READ_DG01));
			QVERIFY(mModel->mAllRights.contains(AccessRight::READ_DG04));
			QVERIFY(mModel->mAllRights.contains(AccessRight::READ_DG10));
			QVERIFY(mModel->mAllRights.contains(AccessRight::READ_DG17));
			QVERIFY(mModel->mSelectedRights.contains(AccessRight::READ_DG01));
			QVERIFY(mModel->mSelectedRights.contains(AccessRight::READ_DG04));
			QVERIFY(mModel->mSelectedRights.contains(AccessRight::READ_DG10));
			QVERIFY(mModel->mSelectedRights.contains(AccessRight::READ_DG17));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
			uniqueRights = QSet<AccessRight>(mModel->mAllRights.constBegin(), mModel->mAllRights.constEnd());
#else
			uniqueRights = mModel->mAllRights.toSet();
#endif
			QCOMPARE(mModel->mSelectedRights, uniqueRights);
		}


		void test_SetOrderedAllRights()
		{
			QSet<AccessRight> rights;
			rights.insert(AccessRight::READ_DG01);
			rights.insert(AccessRight::INSTALL_QUAL_CERT);
			rights.insert(AccessRight::CAN_ALLOWED);
			mModel->setOrderedAllRights(rights);
			QVERIFY(mModel->mAllRights.contains(AccessRight::READ_DG01));
			QVERIFY(mModel->mAllRights.contains(AccessRight::INSTALL_QUAL_CERT));
			QVERIFY(!mModel->mAllRights.contains(AccessRight::CAN_ALLOWED));
		}


		void test_TransferAccessRights()
		{
			mAuthContext->mOptionalAccessRights += mModel->mSelectedRights += AccessRight::INSTALL_QUAL_CERT;
			mAuthContext->mOptionalAccessRights += mModel->mSelectedRights += AccessRight::READ_DG01;
			QVERIFY(!mAuthContext->getEffectiveAccessRights().contains(AccessRight::INSTALL_QUAL_CERT));
			QVERIFY(!mAuthContext->getEffectiveAccessRights().contains(AccessRight::READ_DG01));

			mModel->mAuthContext = mAuthContext;
			mModel->mSelectedRights += AccessRight::INSTALL_QUAL_CERT;
			mModel->mSelectedRights += AccessRight::READ_DG01;

			mModel->transferAccessRights();
			QVERIFY(mAuthContext->getEffectiveAccessRights().contains(AccessRight::INSTALL_QUAL_CERT));
			QVERIFY(mAuthContext->getEffectiveAccessRights().contains(AccessRight::READ_DG01));
		}


		void test_Data_InvalidIndex()
		{
			QCOMPARE(mModel->data(QModelIndex(), 1), QVariant());
		}


		void test_DataOptionalRole()
		{
			QModelIndex index = mModel->createIndex(0, 0);
			QList<AccessRight> rights = {AccessRight::READ_DG01, AccessRight::READ_DG02};
			mModel->mAllRights = rights;

			QCOMPARE(mModel->data(index, ChatModel::ChatRoles::OPTIONAL_ROLE), false);

			mModel->mOptionalRights.insert(AccessRight::READ_DG01);
			QCOMPARE(mModel->data(index, ChatModel::ChatRoles::OPTIONAL_ROLE), true);
		}


		void test_DataSelectedRole()
		{
			QSignalSpy spy(mModel.data(), &ChatModel::dataChanged);
			QModelIndex index = mModel->createIndex(0, 0);
			QList<AccessRight> rights = {AccessRight::READ_DG16, AccessRight::READ_DG02};
			mModel->mAllRights = rights;
			mModel->mSelectedRights = QSet<AccessRight>();

			QCOMPARE(mModel->data(index, ChatModel::ChatRoles::SELECTED_ROLE), false);
			QVERIFY(mModel->setData(index, true, ChatModel::ChatRoles::SELECTED_ROLE));
			QVERIFY(mModel->mSelectedRights.contains(AccessRight::READ_DG16));
			QCOMPARE(spy.count(), 1);
			auto arguments = spy.takeFirst();
			auto role = arguments.at(2).value<QVector<int> >();
			QCOMPARE(arguments.at(0), index);
			QCOMPARE(arguments.at(1), index);
			QCOMPARE(role.at(0), ChatModel::ChatRoles::SELECTED_ROLE);
		}


		void test_DataNameRole()
		{
			mModel->resetContext(mAuthContext);
			QModelIndex index = mModel->createIndex(0, 0);
			QList<AccessRight> rights = {AccessRight::AGE_VERIFICATION, AccessRight::READ_DG15};
			mModel->mAllRights = rights;
			QByteArray content = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1.xml");
			QSharedPointer<DIDAuthenticateEAC1> eac1(static_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(content)));
			mAuthContext->setDidAuthenticateEac1(eac1);
			const QString requiredAge = mAuthContext->getRequiredAge();

			QVERIFY(!requiredAge.isEmpty());
			QCOMPARE(mModel->data(index, ChatModel::ChatRoles::NAME_ROLE), "Age verification (" + requiredAge + ")");
		}


};

QTEST_GUILESS_MAIN(test_ChatModel)
#include "test_ChatModel.moc"
