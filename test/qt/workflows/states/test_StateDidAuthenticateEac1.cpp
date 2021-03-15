/*!
 * \brief Unit test for \ref StateDidAuthenticateEac1
 *
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateDidAuthenticateEac1.h"

#include "MockCardConnectionWorker.h"
#include "TestAuthContext.h"
#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;

class MockEstablishPaceChannelCommand
	: public DidAuthenticateEAC1Command
{
	Q_OBJECT

	public:
		explicit MockEstablishPaceChannelCommand(const QSharedPointer<MockCardConnectionWorker>& pCardConnectionWorker)
			: DidAuthenticateEAC1Command(pCardConnectionWorker)
		{
		}


		void setReturnCode(CardReturnCode pCode)
		{
			mReturnCode = pCode;
		}


};


class test_StateDidAuthenticateEac1
	: public QObject
{
	Q_OBJECT
	QSharedPointer<AuthContext> mAuthContext;

	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1.xml"));
		}


		void cleanup()
		{
			mAuthContext.clear();
		}


		void test_RunStateDidAuthenticateEac1()
		{
			QThread workerThread;
			workerThread.start();

			{
				const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
				worker->moveToThread(&workerThread);
				const QSharedPointer<CardConnection> connection(new CardConnection(worker));
				const QSharedPointer<DIDAuthenticateEAC1> eac1(new DIDAuthenticateEAC1());
				const EstablishPaceChannelOutput output;
				mAuthContext->setCardConnection(connection);
				mAuthContext->setDidAuthenticateEac1(eac1);
				mAuthContext->setPaceOutputData(output);

				StateDidAuthenticateEac1 state(mAuthContext);
				state.run();
				QCOMPARE(state.mConnections.size(), 1);
			}

			workerThread.quit();
			workerThread.wait();
		}


		void test_OnCardCommandDone()
		{
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			QSharedPointer<MockEstablishPaceChannelCommand> command(new MockEstablishPaceChannelCommand(worker));
			StateDidAuthenticateEac1 state(mAuthContext);

			QSignalSpy spyContinue(&state, &StateDidAuthenticateEac1::fireContinue);
			QSignalSpy spyAbort(&state, &StateDidAuthenticateEac1::fireAbort);

			command->setReturnCode(CardReturnCode::UNKNOWN);
			mAuthContext->setStatus(GlobalStatus::Code::No_Error);
			state.onCardCommandDone(command);
			QCOMPARE(mAuthContext->getStatus().getStatusCode(), GlobalStatus::Code::Unknown_Error);
			QCOMPARE(spyAbort.count(), 1);

			command->setReturnCode(CardReturnCode::OK);
			const QByteArray array(QByteArray::fromHex("9000"));
			command->mChallenge = array;
			QSharedPointer<DIDAuthenticateResponseEAC1> response(new DIDAuthenticateResponseEAC1());
			mAuthContext->setDidAuthenticateResponseEac1(response);
			EstablishPaceChannelOutput output;
			mAuthContext->setPaceOutputData(output);
			QTest::ignoreMessage(QtDebugMsg, "No cvc chain determined, request new cvc list");
			state.onCardCommandDone(command);
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateDidAuthenticateEac1)
#include "test_StateDidAuthenticateEac1.moc"
