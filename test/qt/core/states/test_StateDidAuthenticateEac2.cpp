/*!
 * \brief Unit test for \ref StateDidAuthenticateEac2
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateDidAuthenticateEac2.h"

#include "MockCardConnectionWorker.h"
#include "TestAuthContext.h"
#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;

class MockDidAuthenticateEAC2Command
	: public DidAuthenticateEAC2Command
{
	Q_OBJECT

	public:
		explicit MockDidAuthenticateEAC2Command(const QSharedPointer<MockCardConnectionWorker>& pCardConnectionWorker, const CVCertificateChain& pCvcChain,
				const QString& pEphermalPublicKeyAsHex, const QString& pSignatureAsHex, const QByteArray& pAuthenticatedAuxiliaryDataAsBinary)
			: DidAuthenticateEAC2Command(pCardConnectionWorker, pCvcChain, pEphermalPublicKeyAsHex, pSignatureAsHex, pAuthenticatedAuxiliaryDataAsBinary)
		{
		}


		void setReturnCode(CardReturnCode pCode)
		{
			mReturnCode = pCode;
		}


};


class test_StateDidAuthenticateEac2
	: public QObject
{
	Q_OBJECT
	QSharedPointer<AuthContext> mAuthContext;
	QSharedPointer<StateDidAuthenticateEac2> mState;
	QThread workerThread;

	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1.xml"));
			mState.reset(new StateDidAuthenticateEac2(mAuthContext));
			workerThread.start();
		}


		void cleanup()
		{
			mState.clear();
			mAuthContext.clear();
			workerThread.quit();
			workerThread.wait();
		}


		void test_Run()
		{
			QSignalSpy spy(mState.data(), &StateDidAuthenticateEac2::fireAbort);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const EstablishPaceChannelOutput invalidOutput;

			mAuthContext->setCardConnection(connection);
			mAuthContext->setPaceOutputData(invalidOutput);

			mState->run();
			QCOMPARE(mState->mConnections.size(), 0);
			QCOMPARE(mAuthContext->getStatus(), GlobalStatus::Code::Workflow_No_Permission_Error);
			QCOMPARE(spy.count(), 1);
		}


		void test_OnCardCommandDone_data()
		{
			QTest::addColumn<CardReturnCode>("code");
			QTest::addColumn<GlobalStatus::Code>("status");
			QTest::addColumn<int>("abort");

			QTest::newRow("ok") << CardReturnCode::OK << GlobalStatus::Code::No_Error << 0;
			QTest::newRow("command_failed") << CardReturnCode::COMMAND_FAILED << GlobalStatus::Code::Workflow_Card_Removed << 1;
			QTest::newRow("undefined") << CardReturnCode::UNDEFINED << GlobalStatus::Code::Workflow_No_Permission_Error << 1;
		}


		void test_OnCardCommandDone()
		{
			QFETCH(CardReturnCode, code);
			QFETCH(GlobalStatus::Code, status);
			QFETCH(int, abort);

			QSignalSpy spyAbort(mState.data(), &StateDidAuthenticateEac2::fireAbort);
			QSignalSpy spyContinue(mState.data(), &StateDidAuthenticateEac2::fireContinue);

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const CVCertificateChain certificate;
			const QString key("key");
			const QString signature("signature");
			const QByteArray auxiliaryData("data");
			const QSharedPointer<DIDAuthenticateResponseEAC2> response(new DIDAuthenticateResponseEAC2());
			const QSharedPointer<MockDidAuthenticateEAC2Command> command(new MockDidAuthenticateEAC2Command(worker, certificate, key, signature, auxiliaryData));

			command->setReturnCode(code);
			mAuthContext->setDidAuthenticateResponseEac2(response);

			mState->onCardCommandDone(command);
			QCOMPARE(mAuthContext->getStatus(), status);
			QCOMPARE(spyAbort.count(), abort);
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateDidAuthenticateEac2)
#include "test_StateDidAuthenticateEac2.moc"
