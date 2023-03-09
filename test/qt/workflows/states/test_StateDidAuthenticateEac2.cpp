/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit test for \ref StateDidAuthenticateEac2
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
				const QByteArray& pEphermalPublicKeyAsHex, const QByteArray& pSignatureAsHex, const QByteArray& pAuthenticatedAuxiliaryDataAsBinary)
			: DidAuthenticateEAC2Command(pCardConnectionWorker, pCvcChain, pEphermalPublicKeyAsHex, pSignatureAsHex, pAuthenticatedAuxiliaryDataAsBinary, QByteArray())
		{
		}


		void setMockReturnCode(CardReturnCode pReturnCode)
		{
			setReturnCode(pReturnCode);
		}


};

Q_DECLARE_METATYPE(std::optional<FailureCode>)

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
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Did_Authenticate_Eac2_Invalid_Cvc_Chain);
			QCOMPARE(spy.count(), 1);
		}


		void test_OnCardCommandDone_data()
		{
			QTest::addColumn<CardReturnCode>("code");
			QTest::addColumn<GlobalStatus::Code>("status");
			QTest::addColumn<int>("abort");
			QTest::addColumn<std::optional<FailureCode>>("failureCode");

			QTest::newRow("ok") << CardReturnCode::OK << GlobalStatus::Code::No_Error << 0 << std::optional<FailureCode>();
			QTest::newRow("command_failed") << CardReturnCode::COMMAND_FAILED << GlobalStatus::Code::Workflow_Card_Removed << 1 << std::optional<FailureCode>(FailureCode::Reason::Did_Authenticate_Eac2_Card_Command_Failed);
			QTest::newRow("undefined") << CardReturnCode::UNDEFINED << GlobalStatus::Code::Workflow_No_Permission_Error << 1 << std::optional<FailureCode>(FailureCode::Reason::Did_Authenticate_Eac2_Card_Command_Failed);
		}


		void test_OnCardCommandDone()
		{
			QFETCH(CardReturnCode, code);
			QFETCH(GlobalStatus::Code, status);
			QFETCH(int, abort);
			QFETCH(std::optional<FailureCode>, failureCode);

			QSignalSpy spyAbort(mState.data(), &StateDidAuthenticateEac2::fireAbort);
			QSignalSpy spyContinue(mState.data(), &StateDidAuthenticateEac2::fireContinue);

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const CVCertificateChain certificate;
			const QByteArray key("key");
			const QByteArray signature("signature");
			const QByteArray auxiliaryData("data");
			const QSharedPointer<DIDAuthenticateResponseEAC2> response(new DIDAuthenticateResponseEAC2());
			const QSharedPointer<MockDidAuthenticateEAC2Command> command(new MockDidAuthenticateEAC2Command(worker, certificate, key, signature, auxiliaryData));

			command->setMockReturnCode(code);
			mAuthContext->setDidAuthenticateResponseEac2(response);

			mState->onCardCommandDone(command);
			QCOMPARE(mAuthContext->getStatus(), status);
			QCOMPARE(spyAbort.count(), abort);
			QCOMPARE(spyContinue.count(), 1 - abort);

			const auto& actualFailureCode = mAuthContext->getFailureCode();
			QCOMPARE(actualFailureCode, failureCode);
			if (actualFailureCode.has_value())
			{
				QVERIFY(actualFailureCode->getFailureInfoMap().contains(FailureCode::Info::Card_Return_Code));
			}
		}


};

QTEST_GUILESS_MAIN(test_StateDidAuthenticateEac2)
#include "test_StateDidAuthenticateEac2.moc"
