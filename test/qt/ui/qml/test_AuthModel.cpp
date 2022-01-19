/*!
 * \brief Unit tests for \ref AuthModel
 *
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "AuthModel.h"

#include "context/AuthContext.h"

#include "paos/retrieve/DidAuthenticateEac1Parser.h"

#include "TestFileHelper.h"

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
			const auto model = Env::getSingleton<AuthModel>();
			const QSharedPointer<AuthContext> context(new AuthContext(nullptr));

			QSignalSpy spyCurrentStateChanged(model, &WorkflowModel::fireCurrentStateChanged);
			QSignalSpy spyTransactionInfoChanged(model, &AuthModel::fireTransactionInfoChanged);

			model->resetAuthContext(context);
			QCOMPARE(spyCurrentStateChanged.count(), 1);
			QCOMPARE(spyTransactionInfoChanged.count(), 0);

			QByteArray content = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1.xml");
			QSharedPointer<DIDAuthenticateEAC1> eac1(static_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(content)));
			context->setDidAuthenticateEac1(eac1);
			QCOMPARE(model->getTransactionInfo(), QString("this is a test for TransactionInfo"));
			model->resetAuthContext(context);
			QVERIFY(model->getTransactionInfo().isEmpty());
			Q_EMIT context->fireDidAuthenticateEac1Changed();
			QCOMPARE(model->getTransactionInfo(), QString("this is a test for TransactionInfo"));
			QCOMPARE(spyCurrentStateChanged.count(), 2);
			QCOMPARE(spyTransactionInfoChanged.count(), 3);
		}


};

QTEST_GUILESS_MAIN(test_AuthModel)
#include "test_AuthModel.moc"
