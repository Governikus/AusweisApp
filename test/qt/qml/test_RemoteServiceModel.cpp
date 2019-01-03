/*!
 * \brief Unit tests for \ref ProviderModel
 *
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceModel.h"

#include "context/RemoteServiceContext.h"
#include "MockRemoteServer.h"

#include <QDebug>
#include <QtTest>


using namespace governikus;

class test_RemoteServiceModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::setCreator<RemoteServer*>(std::function<RemoteServer*()>([&] {
						return new MockRemoteServer();
					}));
		}


		void test_ReaderPlugInType()
		{
			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			RemoteServiceModel model;
			const ReaderManagerPlugInType input1 = ReaderManagerPlugInType::NFC;
			const ReaderManagerPlugInType input2 = ReaderManagerPlugInType::UNKNOWN;

			model.setReaderPlugInType(input1);
			QCOMPARE(model.getReaderPlugInType(), ReaderManagerPlugInType::UNKNOWN);

			model.resetContext(context);
			model.setReaderPlugInType(input1);
			QCOMPARE(model.getReaderPlugInType(), input1);

			model.setReaderPlugInType(input2);
			QCOMPARE(model.getReaderPlugInType(), ReaderManagerPlugInType::UNKNOWN);
		}


		void test_ErrorMessage()
		{
			RemoteServiceModel model;

			QCOMPARE(model.getErrorMessage(true, true, true), QString());
			QCOMPARE(model.getErrorMessage(false, true, true), tr("NFC is not available on your device."));
			QCOMPARE(model.getErrorMessage(true, false, true), tr("Please enable NFC to use the remote service."));
			QCOMPARE(model.getErrorMessage(true, true, false), tr("Please connect your WiFi to use the remote service."));
		}


		void test_CancelPasswordRequest()
		{
			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			RemoteServiceModel model;
			QSignalSpy spy(context.data(), &RemoteServiceContext::fireCancelPasswordRequest);

			model.cancelPasswordRequest();
			QCOMPARE(spy.count(), 0);

			model.mContext = context;
			model.cancelPasswordRequest();
			QCOMPARE(spy.count(), 1);
		}


		void test_Running()
		{
			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			RemoteServiceModel model;
			QSignalSpy spyStartWorkflow(&model, &RemoteServiceModel::fireStartWorkflow);
			QSignalSpy spyCancelWorkflow(context.data(), &RemoteServiceContext::fireCancelWorkflow);
			QSignalSpy spyIsRunningChanged(&model, &RemoteServiceModel::fireIsRunningChanged);

			QVERIFY(!model.isRunning());

			model.mContext = context;
			QVERIFY(!model.isRunning());

			context->getRemoteServer()->start(QString());
			QVERIFY(model.isRunning());
			model.setRunning(false);
			QCOMPARE(spyIsRunningChanged.count(), 1);
			QCOMPARE(spyCancelWorkflow.count(), 1);

			context->getRemoteServer()->stop();
			model.setRunning(true);
			QCOMPARE(spyIsRunningChanged.count(), 2);
			QCOMPARE(spyStartWorkflow.count(), 1);
		}


		void test_ResetContext()
		{
			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			RemoteServiceModel model;

			QSignalSpy spyConnectedChanged(&model, &RemoteServiceModel::fireConnectedChanged);
			QSignalSpy spyCurrentStateChanged(&model, &WorkflowModel::fireCurrentStateChanged);
			QSignalSpy spyIsRunningChanged(&model, &RemoteServiceModel::fireIsRunningChanged);
			QSignalSpy spyPskChanged(&model, &RemoteServiceModel::firePskChanged);
			QSignalSpy spyConnectedClientDeviceNameChanged(&model, &RemoteServiceModel::fireConnectedClientDeviceNameChanged);

			model.resetContext(context);
			QCOMPARE(model.mContext, context);
			QCOMPARE(spyCurrentStateChanged.count(), 1);
			QCOMPARE(spyConnectedChanged.count(), 1);

			Q_EMIT context->fireStateChanged(QString());
			QCOMPARE(spyIsRunningChanged.count(), 1);

			Q_EMIT context->getRemoteServer()->firePskChanged(QByteArray());
			QCOMPARE(spyPskChanged.count(), 1);

			Q_EMIT context->getRemoteServer()->fireConnectedChanged(true);
			QCOMPARE(spyConnectedChanged.count(), 2);
			QCOMPARE(spyConnectedClientDeviceNameChanged.count(), 1);
		}


		void test_SetPairing()
		{
			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			RemoteServiceModel model;
			model.mContext = context;
			const QSharedPointer<MockRemoteServer> server = qSharedPointerCast<MockRemoteServer>(context->getRemoteServer());

			model.setPairing(true);
			QVERIFY(server->getPairing());

			model.setPairing(false);
			QVERIFY(!server->getPairing());
		}


};

QTEST_GUILESS_MAIN(test_RemoteServiceModel)
#include "test_RemoteServiceModel.moc"
