/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"

#include "Converter.h"
#include "MockCardConnectionWorker.h"
#include "TestHookThread.h"

#include <QtTest>


using namespace governikus;


class test_CardConnection
	: public QObject
{
	Q_OBJECT

	private:
		QScopedPointer<TestHookThread> workerThread;

	private Q_SLOTS:
		void init()
		{
			workerThread.reset(new TestHookThread());
		}


		void cleanup()
		{
			workerThread.reset();
		}


		void test_CreateUpdateRetryCounterCommand()
		{
			const auto& worker = MockCardConnectionWorker::create(workerThread.data());
			CardConnection connection(worker);
			const QString slotHandle = QStringLiteral("slot handle");

			UpdateRetryCounterCommand* command = connection.createUpdateRetryCounterCommand(slotHandle);
			workerThread->waitForDestruction(command);
			QCOMPARE(command->mCardConnectionWorker, worker);
			QCOMPARE(command->getSlotHandle(), slotHandle);

			command->run();
		}


		void test_CreateResetRetryCounterCommand()
		{
			const auto& worker = MockCardConnectionWorker::create(workerThread.data());
			CardConnection connection(worker);

			ResetRetryCounterCommand* command = connection.createResetRetryCounterCommand();
			workerThread->waitForDestruction(command);
			QCOMPARE(command->mCardConnectionWorker, worker);

			command->run();
		}


		void test_CreateDestroyPaceChannelCommand()
		{
			const auto& worker = MockCardConnectionWorker::create(workerThread.data());
			CardConnection connection(worker);
			const QString slotHandle = QStringLiteral("slot handle");

			DestroyPaceChannelCommand* command = connection.createDestroyPaceChannelCommand(slotHandle);
			workerThread->waitForDestruction(command);
			QCOMPARE(command->mCardConnectionWorker, worker);
			QCOMPARE(command->getSlotHandle(), slotHandle);

			command->run();
		}


		void test_CreateDidAuthenticateEAC1Command()
		{
			const auto& worker = MockCardConnectionWorker::create(workerThread.data());
			CardConnection connection(worker);

			DidAuthenticateEAC1Command* command = connection.createDidAuthenticateEAC1Command();
			workerThread->waitForDestruction(command);
			QCOMPARE(command->mCardConnectionWorker, worker);

			command->run();
		}


		void test_CreateDidAuthenticateEAC2Command()
		{
			const auto& worker = MockCardConnectionWorker::create(workerThread.data());
			CardConnection connection(worker);

			const auto& dv = Converter::certificatefromHex(
					"7F2181E77F4E81A05F290100420E44455445535465494430303030347F494F060A04007F000702020202038641045B1CB1090D5064FE0AEE21BD95C062AB94C9"
					"52F7D64274EAF004C2E3DA4ABFDA2D0108B2545CEAF8BAF40BDEA1D161BE8950B3353BFD267F0674EC9ACABA71D05F2010444544566549444450535430303033"
					"357F4C12060904007F0007030102025305400513FF875F25060104000201015F24060104000501025F37409389856B8DA9956BA3E9894812BA87F86664666055"
					"7131EF618349BF145A0826A8FB4A9BE22589CDE868B074C3FDA73DB84F9FDC84B87B3896702E42B4FE86E7");
			const auto& at = Converter::certificatefromHex(
					"7f218201487f4e8201005f2901004210444544566549444450535430303033357f494f060a04007f0007020202020386410400c7dedb2117e45acf998e9d3ed3"
					"4883e0617d1614b60430ca1df1d2ecc96bc214d97451588ef706deaf7f68163f7c8eaadf9ea028f0f8bf5d0dd67b650907175f200e444544454d4f4445563030"
					"3033387f4c12060904007f0007030102025305000513ff875f25060104000301035f2406010400040101655e732d060904007f000703010301802012ca9d0a51"
					"df9297eaba7ebe9ab49df2f4cf83e0dbb02772efad89c8ad75fccd732d060904007f0007030103028020cb1e1940159f11dc96845b87c23b86f9baa755a789a9"
					"14bbd5b8fa9784019d1c5f37407ab2b3c8de4b3f7136f7da91ccac25b26aec5bc35ad0b603fa2ffe50cea49f785614ad3fb2eff1719971fbcabc95f95a9f601f"
					"9017bd73952b45e645b90b774f");
			const CVCertificateChain chain({dv, at});
			const QByteArray publicKey("0000");
			const QByteArray signature("0000");
			const QByteArray authenticatedAuxiliaryData("0000");
			const QByteArray pin("123456");

			DidAuthenticateEAC2Command* command = connection.createDidAuthenticateEAC2Command(chain, publicKey, signature, authenticatedAuxiliaryData, pin);
			workerThread->waitForDestruction(command);
			QCOMPARE(command->mCardConnectionWorker, worker);
			QCOMPARE(command->mCvcChain, chain);
			QCOMPARE(command->mEphemeralPublicKeyAsHex, publicKey);
			QCOMPARE(command->mSignatureAsHex, signature);
			QCOMPARE(command->mAuthenticatedAuxiliaryDataAsBinary, authenticatedAuxiliaryData);
			QCOMPARE(command->mPin, pin);

			command->run();
		}


		void test_CreateTransmitCommand()
		{
			const auto& worker = MockCardConnectionWorker::create(workerThread.data());
			CardConnection connection(worker);
			const QList<InputAPDUInfo> apduInfos(1);
			const QString slotHandle = QStringLiteral("slot handle");

			TransmitCommand* command = connection.createTransmitCommand(apduInfos, slotHandle);
			workerThread->waitForDestruction(command);
			QCOMPARE(command->mCardConnectionWorker, worker);
			QCOMPARE(command->getSlotHandle(), slotHandle);

			command->run();
		}


};

QTEST_GUILESS_MAIN(test_CardConnection)
#include "test_CardConnection.moc"
