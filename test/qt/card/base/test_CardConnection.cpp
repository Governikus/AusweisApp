/*!
 * \brief Tests for the class CardConnection.
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"

#include "MockCardConnectionWorker.h"

#include <QtTest>

using namespace governikus;

class test_CardConnection
	: public QObject
{
	Q_OBJECT
	QThread workerThread;

	private Q_SLOTS:
		void init()
		{
			workerThread.start();
		}


		void cleanup()
		{
			workerThread.quit();
			workerThread.wait();
		}


		void test_CreateUpdateRetryCounterCommand()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			CardConnection connection(worker);
			UpdateRetryCounterCommand* command;
			command = connection.createUpdateRetryCounterCommand();
			QCOMPARE(command->mCardConnectionWorker, worker);
		}


		void test_CreateDestroyPaceChannelCommand()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			CardConnection connection(worker);
			DestroyPaceChannelCommand* command;
			command = connection.createDestroyPaceChannelCommand();
			QCOMPARE(command->mCardConnectionWorker, worker);
		}


		void test_CreateDidAuthenticateEAC1Command()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			CardConnection connection(worker);
			DidAuthenticateEAC1Command* command;
			command = connection.createDidAuthenticateEAC1Command();
			QCOMPARE(command->mCardConnectionWorker, worker);
		}


		void test_CreateDidAuthenticateEAC2Command()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			CardConnection connection(worker);
			const CVCertificateChain chain;
			const QString publicKey = QStringLiteral("0000");
			const QString signature = QStringLiteral("0000");
			const QByteArray authenticatedAuxiliaryData("0000");
			DidAuthenticateEAC2Command* command;
			command = connection.createDidAuthenticateEAC2Command(chain, publicKey, signature, authenticatedAuxiliaryData);
			QCOMPARE(command->mCardConnectionWorker, worker);
			QCOMPARE(command->mCvcChain, chain);
			QCOMPARE(command->mEphemeralPublicKeyAsHex, publicKey);
			QCOMPARE(command->mSignatureAsHex, signature);
			QCOMPARE(command->mAuthenticatedAuxiliaryDataAsBinary, authenticatedAuxiliaryData);
		}


		void test_CreateTransmitCommand()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			CardConnection connection(worker);
			const QVector<InputAPDUInfo> apduInfos;
			const QString slotHandle = QStringLiteral("slot handle");
			TransmitCommand* command;
			command = connection.createTransmitCommand(apduInfos, slotHandle);
			QCOMPARE(command->mCardConnectionWorker, worker);
			QCOMPARE(command->mSlotHandle, slotHandle);
		}


};

QTEST_GUILESS_MAIN(test_CardConnection)
#include "test_CardConnection.moc"
