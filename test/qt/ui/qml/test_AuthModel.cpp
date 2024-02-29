/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref AuthModel
 */

#include "AuthModel.h"

#include "context/AuthContext.h"

#include "paos/retrieve/DidAuthenticateEac1Parser.h"

#include "TestFileHelper.h"

#include <QDebug>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_AuthModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_ResetContext()
		{
			const auto model = Env::getSingleton<AuthModel>();
			const QSharedPointer<AuthContext> context(new AuthContext());

			QSignalSpy spyWorkflowStarted(model, &WorkflowModel::fireWorkflowStarted);
			QSignalSpy spyCurrentStateChanged(model, &WorkflowModel::fireCurrentStateChanged);
			QSignalSpy spyStateEntered(model, &WorkflowModel::fireStateEntered);
			QSignalSpy spyTransactionInfoChanged(model, &AuthModel::fireTransactionInfoChanged);

			model->resetAuthContext(context);
			QCOMPARE(spyWorkflowStarted.count(), 1);
			QCOMPARE(spyCurrentStateChanged.count(), 1);
			QCOMPARE(spyStateEntered.count(), 0);
			QCOMPARE(spyTransactionInfoChanged.count(), 0);

			QByteArray content = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1.xml"_L1);
			QSharedPointer<DIDAuthenticateEAC1> eac1(static_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(content)));
			context->setDidAuthenticateEac1(eac1);
			QCOMPARE(model->getTransactionInfo(), "this is a test for TransactionInfo"_L1);
			model->resetAuthContext(context);
			QVERIFY(model->getTransactionInfo().isEmpty());
			Q_EMIT context->fireDidAuthenticateEac1Changed();
			QCOMPARE(model->getTransactionInfo(), "this is a test for TransactionInfo"_L1);
			QCOMPARE(spyWorkflowStarted.count(), 2);
			QCOMPARE(spyCurrentStateChanged.count(), 2);
			QCOMPARE(spyStateEntered.count(), 0);
			QCOMPARE(spyTransactionInfoChanged.count(), 3);
		}


};

QTEST_MAIN(test_AuthModel)
#include "test_AuthModel.moc"
