/*!
 * \brief Unit tests for \ref AuthModel
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "AuthModel.h"

#include "context/AuthContext.h"

#include "MockActivationContext.h"

#include <QDebug>
#include <QtTest>


using namespace governikus;


class test_AuthModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_ResetContext()
		{
			AuthModel model;
			const QSharedPointer<AuthContext> context(new AuthContext(nullptr));
			const QString transactionInfo = QStringLiteral("info");

			QSignalSpy spyCurrentStateChanged(&model, &WorkflowModel::fireCurrentStateChanged);
			QSignalSpy spyTransactionInfoChanged(&model, &AuthModel::fireTransactionInfoChanged);

			model.resetContext(nullptr);
			QCOMPARE(spyCurrentStateChanged.count(), 1);
			QCOMPARE(spyTransactionInfoChanged.count(), 0);

			model.mTransactionInfo = transactionInfo;
			model.resetContext(context);
			QVERIFY(model.mTransactionInfo.isEmpty());
			QCOMPARE(spyCurrentStateChanged.count(), 2);
			QCOMPARE(spyTransactionInfoChanged.count(), 1);
		}


		void test_OnDidAuthenticateEac1Changed()
		{
			AuthModel model;
			const QSharedPointer<AuthContext> context(new AuthContext(nullptr));
			const QString transactionInfo = QStringLiteral("info");
			const QSharedPointer<DIDAuthenticateEAC1> eac1(new DIDAuthenticateEAC1());
			Eac1InputType type;
			const QString info = QStringLiteral("new info");
			QSignalSpy spy(&model, &AuthModel::fireTransactionInfoChanged);

			model.mContext = context;
			model.mTransactionInfo = transactionInfo;
			model.onDidAuthenticateEac1Changed();
			QCOMPARE(model.mTransactionInfo, QString());
			QCOMPARE(spy.count(), 1);

			type.setTransactionInfo(info);
			eac1->setEac1InputType(type);
			context->setDidAuthenticateEac1(eac1);
			model.onDidAuthenticateEac1Changed();
			QCOMPARE(model.mTransactionInfo, info);
			QCOMPARE(spy.count(), 2);
		}


};

QTEST_GUILESS_MAIN(test_AuthModel)
#include "test_AuthModel.moc"
