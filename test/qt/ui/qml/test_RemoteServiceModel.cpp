/*!
 * \brief Unit tests for \ref ProviderModel
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceModel.h"

#include "context/RemoteServiceContext.h"
#include "EstablishPaceChannelParser.h"
#include "MockRemoteServer.h"

#include <QDebug>
#include <QtTest>


using namespace governikus;

class test_RemoteServiceModel
	: public QObject
{
	Q_OBJECT
	RemoteServiceModel * mModel;
	QSharedPointer<RemoteServiceContext> mContext;

	static QByteArray createDataToParse(const PacePasswordId& pinId)
	{
		const QByteArray chat = QByteArray::fromHex("7F4C12060904007F00070301020253050000000F0F");
		const QByteArray certDescription = QByteArray::fromHex("30 8202A4"
															   "        06 0A 04007F00070301030103"
															   "        A1 0E 0C0C442D547275737420476D6248"
															   "        A3 3A 0C38476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E"
															   "        A5 820248"
															   "            04 820244 4E616D652C20416E7363687269667420756E6420452D4D61696C2D4164726573736520646573204469656E737465616E626965746572733A0D0A476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E0D0A57696C68656C6D73747261C39F652034332F3433670D0A3130313137204265726C696E0D0A6265726C696E406764762E64650D0A0D0A4765736368C3A46674737A7765636B3A0D0A2D52656769737472696572756E6720756E64204C6F67696E20616D204744562D4D616B6C6572706F7274616C2D0D0A0D0A48696E7765697320617566206469652066C3BC722064656E204469656E737465616E626965746572207A757374C3A46E646967656E205374656C6C656E2C20646965206469652045696E68616C74756E672064657220566F7273636872696674656E207A756D20446174656E73636875747A206B6F6E74726F6C6C696572656E3A0D0A4265726C696E6572204265617566747261677465722066C3BC7220446174656E73636875747A20756E6420496E666F726D6174696F6E7366726569686569740D0A416E20646572205572616E696120342D31300D0A3130373837204265726C696E0D0A3033302F3133382038392D300D0A6D61696C626F7840646174656E73636875747A2D6265726C696E2E64650D0A687474703A2F2F7777772E646174656E73636875747A2D6265726C696E2E64650D0A416E737072656368706172746E65723A2044722E20416C6578616E64657220446978");

		EstablishPaceChannel builder;
		builder.setPasswordId(pinId);
		builder.setChat(chat);
		builder.setCertificateDescription(certDescription);
		const auto command = builder.createCommandDataCcid();

		return command.getBuffer();
	}

	private Q_SLOTS:
		void initTestCase()
		{
			Env::setCreator<RemoteServer*>(std::function<RemoteServer* ()>([&] {
						return new MockRemoteServer();
					}));
			mModel = Env::getSingleton<RemoteServiceModel>();
		}


		void init()
		{
			mContext.reset(new RemoteServiceContext());
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
			QSignalSpy spy(mContext.data(), &RemoteServiceContext::fireCancelPasswordRequest);

			mModel->cancelPasswordRequest();
			QCOMPARE(spy.count(), 0);

			mModel->resetRemoteServiceContext(mContext);
			mModel->cancelPasswordRequest();
			QCOMPARE(spy.count(), 1);
		}


		void test_Running()
		{
			QSignalSpy spyStartWorkflow(mModel, &RemoteServiceModel::fireStartWorkflow);
			QSignalSpy spyCancelWorkflow(mContext.data(), &RemoteServiceContext::fireCancelWorkflow);
			QSignalSpy spyIsRunningChanged(mModel, &RemoteServiceModel::fireIsRunningChanged);

			QVERIFY(!mModel->isRunning());

			mModel->resetRemoteServiceContext(mContext);
			QVERIFY(!mModel->isRunning());

			mContext->getRemoteServer()->start(QString());
			QVERIFY(mModel->isRunning());
			mModel->setRunning(false);
			QCOMPARE(spyIsRunningChanged.count(), 1);
			QCOMPARE(spyCancelWorkflow.count(), 1);

			mContext->getRemoteServer()->stop();
			mModel->setRunning(true);
			QCOMPARE(spyIsRunningChanged.count(), 2);
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

			Q_EMIT mContext->getRemoteServer()->firePskChanged(QByteArray());
			QCOMPARE(spyPskChanged.count(), 1);

			Q_EMIT mContext->getRemoteServer()->fireConnectedChanged(true);
			QCOMPARE(spyConnectedChanged.count(), 2);
			QCOMPARE(spyConnectedClientDeviceNameChanged.count(), 1);
		}


		void test_SetPairing()
		{
			mModel->resetRemoteServiceContext(mContext);
			const QSharedPointer<MockRemoteServer> server = qSharedPointerCast<MockRemoteServer>(mContext->getRemoteServer());

			mModel->setPairing(true);
			QVERIFY(server->getPairing());

			mModel->setPairing(false);
			QVERIFY(!server->getPairing());
		}


		void test_GetPasswordTypeContextIsNull()
		{
			QCOMPARE(mModel->getPasswordType(), QString());
		}


		void test_GetPassword_data()
		{
			QTest::addColumn<PacePasswordId>("password");
			QTest::addColumn<QString>("output");

			QTest::newRow("pin") << PacePasswordId::PACE_PIN << QString("PIN");
			QTest::newRow("can") << PacePasswordId::PACE_CAN << QString("CAN");
			QTest::newRow("puk") << PacePasswordId::PACE_PUK << QString("PUK");
			QTest::newRow("unknown") << PacePasswordId::UNKNOWN << QString();
		}


		void test_GetPassword()
		{
			QFETCH(PacePasswordId, password);
			QFETCH(QString, output);

			auto data = createDataToParse(password);
			QSharedPointer<const IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel(QString(), data));
			mContext->setEstablishPaceChannelMessage(message);
			mModel->resetRemoteServiceContext(mContext);
			QCOMPARE(mModel->getPasswordType(), output);
		}


};

QTEST_GUILESS_MAIN(test_RemoteServiceModel)
#include "test_RemoteServiceModel.moc"
