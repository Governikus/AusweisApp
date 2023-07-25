/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref ProviderModel
 */

#include "RemoteServiceModel.h"

#include "MockIfdServer.h"
#include "context/IfdServiceContext.h"

#include <QDebug>
#include <QtTest>


using namespace governikus;

Q_DECLARE_METATYPE(EstablishPaceChannel)

class test_RemoteServiceModel
	: public QObject
{
	Q_OBJECT

	private:
		RemoteServiceModel* mModel = nullptr;
		QSharedPointer<IfdServiceContext> mContext;

	private Q_SLOTS:
		void initTestCase()
		{
			mModel = Env::getSingleton<RemoteServiceModel>();
		}


		void init()
		{
			mContext.reset(new IfdServiceContext(QSharedPointer<IfdServer>(new MockIfdServer())));
		}


		void cleanup()
		{
			mModel->resetRemoteServiceContext();
			mContext.clear();
		}


		void test_ReaderPlugInType()
		{
			const ReaderManagerPlugInType input1 = ReaderManagerPlugInType::NFC;
			const ReaderManagerPlugInType input2 = ReaderManagerPlugInType::UNKNOWN;

			mModel->setReaderPlugInType(input1);
			QCOMPARE(mModel->getReaderPlugInType(), ReaderManagerPlugInType::UNKNOWN);

			mModel->resetRemoteServiceContext(mContext);
			mModel->setReaderPlugInType(input1);
			QCOMPARE(mModel->getReaderPlugInType(), input1);

			mModel->setReaderPlugInType(input2);
			QCOMPARE(mModel->getReaderPlugInType(), ReaderManagerPlugInType::UNKNOWN);
		}


		void test_CancelPasswordRequest()
		{
			QSignalSpy spy(mContext.data(), &IfdServiceContext::fireCancelPasswordRequest);

			mModel->cancelPasswordRequest();
			QCOMPARE(spy.count(), 0);

			mModel->resetRemoteServiceContext(mContext);
			mModel->cancelPasswordRequest();
			QCOMPARE(spy.count(), 1);
		}


		void test_Running()
		{
			QSignalSpy spyStartWorkflow(mModel, &RemoteServiceModel::fireStartWorkflow);
			QSignalSpy spyCancelWorkflow(mContext.data(), &IfdServiceContext::fireCancelWorkflow);
			QSignalSpy spyIsRunningChanged(mModel, &RemoteServiceModel::fireIsRunningChanged);

			QVERIFY(!mModel->isRunning());

			mModel->resetRemoteServiceContext(mContext);
			QCOMPARE(spyIsRunningChanged.count(), 1);
			QVERIFY(!mModel->isRunning());

			mContext->getIfdServer()->start(QString());
			QVERIFY(mModel->isRunning());
			mModel->setRunning(false);
			QCOMPARE(spyIsRunningChanged.count(), 2);
			QCOMPARE(spyCancelWorkflow.count(), 1);

			mContext->getIfdServer()->stop();
			mModel->setRunning(true);
			QCOMPARE(spyIsRunningChanged.count(), 3);
			QCOMPARE(spyStartWorkflow.count(), 1);
		}


		void test_ResetContext()
		{
			QSignalSpy spyConnectedChanged(mModel, &RemoteServiceModel::fireConnectedChanged);
			QSignalSpy spyCurrentStateChanged(mModel, &WorkflowModel::fireCurrentStateChanged);
			QSignalSpy spyIsRunningChanged(mModel, &RemoteServiceModel::fireIsRunningChanged);
			QSignalSpy spyPskChanged(mModel, &RemoteServiceModel::firePskChanged);
			QSignalSpy spyConnectedClientDeviceNameChanged(mModel, &RemoteServiceModel::fireConnectionInfoChanged);

			mModel->resetRemoteServiceContext(mContext);
			QCOMPARE(spyCurrentStateChanged.count(), 1);
			QCOMPARE(spyConnectedChanged.count(), 1);

			Q_EMIT mContext->fireStateChanged(QString());
			QCOMPARE(spyIsRunningChanged.count(), 1);

			Q_EMIT mContext->getIfdServer()->firePskChanged(QByteArray());
			QCOMPARE(spyPskChanged.count(), 1);

			Q_EMIT mContext->getIfdServer()->fireConnectedChanged(true);
			QCOMPARE(spyConnectedChanged.count(), 2);
			QCOMPARE(spyConnectedClientDeviceNameChanged.count(), 1);
		}


		void test_SetPairing()
		{
			mModel->resetRemoteServiceContext(mContext);
			const QSharedPointer<MockIfdServer> server = qSharedPointerCast<MockIfdServer>(mContext->getIfdServer());

			mModel->setPairing(true);
			QVERIFY(server->getPairing());

			mModel->setPairing(false);
			QVERIFY(!server->getPairing());
		}


		void test_TransactionInfo()
		{
			mModel->resetRemoteServiceContext(mContext);
			QCOMPARE(mModel->getTransactionInfo(), QString());
		}


		void test_getSupportedReaderPlugInTypes()
		{
			QVector<ReaderManagerPlugInType> supportedPlugIns {ReaderManagerPlugInType::NFC};
#if __has_include("SmartManager.h")
			supportedPlugIns << ReaderManagerPlugInType::SMART;
#endif
			QCOMPARE(mModel->getSupportedReaderPlugInTypes(), supportedPlugIns);
		}


		void test_DisplayText_data()
		{
			QTest::addColumn<QString>("inputText");
			QTest::addColumn<QString>("outputText");
			QTest::addColumn<int>("expectedPercentage");

			QTest::newRow("positive two digits") << "Dummy Text\n15%" << "Dummy Text" << 15;
			QTest::newRow("positive two digits with space") << "Dummy Text\n15 %" << "Dummy Text" << 15;
			QTest::newRow("negative two digits") << "Dummy Text\n-15%" << "Dummy Text\n-15%" << 0;
			QTest::newRow("positive two digits with percentage") << "Dummy Text\n15%" << "Dummy Text" << 15;
			QTest::newRow("Whitespace input") << "   " << "   " << 0;
			QTest::newRow("Special characters") << "!@#$%^&*()" << "!@#$%^&*()" << 0;
			QTest::newRow("Digits only") << "12345" << "12345" << 0;
			QTest::newRow("Single Digit") << "Dummy Text\n1%" << "Dummy Text" << 1;
			QTest::newRow("Triple Digit") << "Dummy Text\n111%" << "Dummy Text\n111%" << 0;
			QTest::newRow("100 Percent") << "Dummy Text\n100%" << "Dummy Text" << 100;
			QTest::newRow("Quadruple Digit") << "Dummy Text\n1111%" << "Dummy Text\n1111%" << 0;
			QTest::newRow("Real Number") << "Dummy Text\n11.1%" << "Dummy Text\n11.1%" << 0;
			QTest::newRow("Zero Digit without Space") << "%" << "%" << 0;
			QTest::newRow("Single Digit without Space") << "1%" << "" << 1;
			QTest::newRow("Double Digit without Space") << "42%" << "" << 42;
			QTest::newRow("Triple Digit without Space") << "100%" << "" << 100;
			QTest::newRow("Zero Digit with Space") << " %" << " %" << 0;
			QTest::newRow("Single Digit with Space") << "1 %" << "" << 1;
			QTest::newRow("Double Digit with Space") << "42 %" << "" << 42;
			QTest::newRow("Triple Digit with Space") << "100 %" << "" << 100;

		}


		void test_DisplayText()
		{
			QFETCH(QString, inputText);
			QFETCH(QString, outputText);
			QFETCH(int, expectedPercentage);

			QSignalSpy spyDisplayTextChanged(mModel, &RemoteServiceModel::fireDisplayTextChanged);

			mModel->resetRemoteServiceContext(mContext);
			mContext->setDisplayText(inputText);
			QCOMPARE(spyDisplayTextChanged.count(), 1);
			QCOMPARE(mModel->getDisplayText(), outputText);
			QCOMPARE(mModel->getPercentage(), expectedPercentage);
		}


		void test_DisplayTextSignals()
		{
			QSignalSpy spyDisplayTextChanged(mModel, &RemoteServiceModel::fireDisplayTextChanged);

			QCOMPARE(mModel->getDisplayText(), "");
			QCOMPARE(mModel->getPercentage(), 0);

			mModel->resetRemoteServiceContext(mContext);
			QCOMPARE(spyDisplayTextChanged.count(), 0);
			mContext->setDisplayText("Dummy");
			QCOMPARE(spyDisplayTextChanged.count(), 1);
			mContext->setDisplayText("Dummy");
			QCOMPARE(spyDisplayTextChanged.count(), 1);

		}


};

QTEST_MAIN(test_RemoteServiceModel)
#include "test_RemoteServiceModel.moc"
