/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Tests for the class CardConnection.
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
			UpdateRetryCounterCommand* command = connection.createUpdateRetryCounterCommand();
			command->deleteLater();
			QCOMPARE(command->mCardConnectionWorker, worker);
		}


		void test_CreateResetRetryCounterCommand()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			CardConnection connection(worker);
			ResetRetryCounterCommand* command = connection.createResetRetryCounterCommand();
			command->deleteLater();
			QCOMPARE(command->mCardConnectionWorker, worker);
		}


		void test_CreateDestroyPaceChannelCommand()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			CardConnection connection(worker);
			DestroyPaceChannelCommand* command = connection.createDestroyPaceChannelCommand();
			command->deleteLater();
			QCOMPARE(command->mCardConnectionWorker, worker);
		}


		void test_CreateDidAuthenticateEAC1Command()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			CardConnection connection(worker);
			DidAuthenticateEAC1Command* command = connection.createDidAuthenticateEAC1Command();
			command->deleteLater();
			QCOMPARE(command->mCardConnectionWorker, worker);
		}


		void test_CreateDidAuthenticateEAC2Command()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			CardConnection connection(worker);
			const CVCertificateChain chain;
			const QByteArray publicKey("0000");
			const QByteArray signature("0000");
			const QByteArray authenticatedAuxiliaryData("0000");
			const QByteArray pin("123456");
			DidAuthenticateEAC2Command* command = connection.createDidAuthenticateEAC2Command(chain, publicKey, signature, authenticatedAuxiliaryData, pin);
			command->deleteLater();
			QCOMPARE(command->mCardConnectionWorker, worker);
			QCOMPARE(command->mCvcChain, chain);
			QCOMPARE(command->mEphemeralPublicKeyAsHex, publicKey);
			QCOMPARE(command->mSignatureAsHex, signature);
			QCOMPARE(command->mAuthenticatedAuxiliaryDataAsBinary, authenticatedAuxiliaryData);
			QCOMPARE(command->mPin, pin);
		}


		void test_CreateTransmitCommand()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			CardConnection connection(worker);
			const QList<InputAPDUInfo> apduInfos;
			const QString slotHandle = QStringLiteral("slot handle");
			TransmitCommand* command = connection.createTransmitCommand(apduInfos, slotHandle);
			command->deleteLater();
			QCOMPARE(command->mCardConnectionWorker, worker);
			QCOMPARE(command->mSlotHandle, slotHandle);
		}


};

QTEST_GUILESS_MAIN(test_CardConnection)
#include "test_CardConnection.moc"
