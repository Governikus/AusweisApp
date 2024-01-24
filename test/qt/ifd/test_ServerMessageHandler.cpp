/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref ServerMessageHandlerImpl
 */

#include "ServerMessageHandler.h"

#include "AppSettings.h"
#include "LogHandler.h"
#include "ReaderManager.h"
#include "ServerMessageHandlerImpl.h"
#include "messages/IfdConnect.h"
#include "messages/IfdConnectResponse.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdDisconnectResponse.h"
#include "messages/IfdError.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdEstablishPaceChannelResponse.h"
#include "messages/IfdModifyPinResponse.h"
#include "messages/IfdStatus.h"
#include "messages/IfdTransmit.h"
#include "messages/IfdTransmitResponse.h"

#include "MockCardConnectionWorker.h"
#include "MockDataChannel.h"
#include "MockReaderManagerPlugIn.h"
#include "TestFileHelper.h"

#include <QSignalSpy>
#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;

Q_DECLARE_METATYPE(ECardApiResult::Minor)

class MockIfdMessageDispatcherServer
	: public IfdDispatcherServer
{
	Q_OBJECT
	QSharedPointer<const IfdMessage> mMessage;

	public:
		explicit MockIfdMessageDispatcherServer(const QSharedPointer<DataChannel>& pDataChannel)
			: IfdDispatcherServer(pDataChannel)
		{
		}


		Q_INVOKABLE void send(const QSharedPointer<const IfdMessage>& pMessage) override
		{
			IfdDispatcherServer::send(pMessage);
			mMessage = pMessage;
		}


		QSharedPointer<const IfdMessage> getMessage()
		{
			return mMessage;
		}


};

class test_ServerMessageHandler
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<MockDataChannel> mDataChannel;
		QPointer<MockIfdMessageDispatcherServer> mDispatcher;


		void removeReaderAndConsumeMessages(const QString& pReaderName)
		{
			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);
			MockReaderManagerPlugIn::getInstance().removeReader(pReaderName);
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void ensureContext(QString& pContextHandle)
		{
			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			const QByteArray establishContextMsg("{\n"
												 "    \"msg\": \"IFDEstablishContext\",\n"
												 "    \"Protocol\": \"IFDInterface_WebSocket_v2\",\n"
												 "    \"UDName\": \"MAC-MINI\"\n"
												 "}");

			mDataChannel->onReceived(establishContextMsg);

			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations
			const QList<QVariant>& establishContextResponseArguments = sendSpy.last();

			const QVariant establishContextResponseVariant = establishContextResponseArguments.at(0);
			QVERIFY(establishContextResponseVariant.canConvert<QByteArray>());
			const IfdEstablishContextResponse establishContextResponse(IfdMessage::parseByteArray(establishContextResponseVariant.toByteArray()));
			QVERIFY(!establishContextResponse.isIncomplete());
			QCOMPARE(establishContextResponse.getType(), IfdMessageType::IFDEstablishContextResponse);

			pContextHandle = establishContextResponse.getContextHandle();
			QVERIFY(!pContextHandle.isEmpty());
		}

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
			const auto readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations

			Env::setCreator<IfdDispatcherServer*>(std::function<IfdDispatcherServer* (const QSharedPointer<DataChannel>& pDataChannel)>([this](const QSharedPointer<DataChannel>){
					mDispatcher = new MockIfdMessageDispatcherServer(mDataChannel);
					return mDispatcher.data();
				}));
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void init()
		{
			mDataChannel.reset(new MockDataChannel());
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void checkLogOnInvalidContext()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			IfdConnectResponse unexpectedMsg(QStringLiteral("RemoteReader"));

			mDataChannel->onReceived(unexpectedMsg.toByteArray(IfdVersion::Version::latest, QStringLiteral("invalidConextHandle")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid context handle received")));
		}


		void checkLogOnUnexpectedMessageWithContext()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QSignalSpy spyContextHandle(mDataChannel.data(), &MockDataChannel::fireSend);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);

			IfdEstablishContext establishContext(IfdVersion::Version::v2, DeviceInfo::getName());
			mDataChannel->onReceived(establishContext.toByteArray(IfdVersion::Version::v2, QString()));

			const QJsonDocument& doc = QJsonDocument::fromJson(spyContextHandle.at(0).at(0).toByteArray());
			const QString& contextHandle = doc.object().value(QLatin1String("ContextHandle")).toString();

			IfdConnectResponse unexpectedMsg(QStringLiteral("RemoteReader"));
			mDataChannel->onReceived(unexpectedMsg.toByteArray(IfdVersion::Version::v2, contextHandle));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Received an unexpected message of type: IFDConnectResponse")));
		}


		void checkLogOnInvalidMessage()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);

			mDataChannel->onReceived("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"msg\": \"RANDOM_STUFF\"\n"
									 "}\n");
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received")));
		}


		void testUnexpectedMessagesCauseAnIfdErrorMessage()
		{
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			// We have a context handle: send unexpected messages and verify that an error message is sent back.
			sendSpy.clear();

			ReaderInfo info(QStringLiteral("NFC Reader"));
			info.setMaxApduLength(500);
			info.setBasicReader(true);
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(false);
			const IfdStatus status(info);

			const QByteArrayList serverMessages({
						status.toByteArray(IfdVersion::Version::v2, contextHandle),
						IfdConnectResponse("NFC Reader").toByteArray(IfdVersion::Version::v2, contextHandle),
						IfdDisconnectResponse("NFC Reader").toByteArray(IfdVersion::Version::v2, contextHandle),
						IfdTransmitResponse("NFC Reader", "9000").toByteArray(IfdVersion::Version::v2, contextHandle),
						IfdEstablishPaceChannelResponse("My little Reader", EstablishPaceChannelOutput()).toByteArray(IfdVersion::Version::v2, contextHandle)
					});
			for (const auto& serverMessage : serverMessages)
			{
				mDataChannel->onReceived(serverMessage);
				QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

				const QList<QVariant>& errorMessageArguments = sendSpy.last();
				const QVariant errorMessageVariant = errorMessageArguments.at(0);
				QVERIFY(errorMessageVariant.canConvert<QByteArray>());

				const IfdError errorMessage(IfdMessage::parseByteArray(errorMessageVariant.toByteArray()));
				QVERIFY(!errorMessage.isIncomplete());
				QCOMPARE(errorMessage.getType(), IfdMessageType::IFDError);
				QCOMPARE(errorMessage.getContextHandle(), contextHandle);
				QCOMPARE(errorMessage.getSlotHandle(), QString());
				QVERIFY(errorMessage.resultHasError());
				QCOMPARE(errorMessage.getResultMinor(), ECardApiResult::Minor::AL_Unknown_API_Function);

				sendSpy.clear();
			}
		}


		void ifdConnectForUnconnectedReaderSendsIFDL_UnknownSlot()
		{
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);
			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(IfdMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), IfdMessageType::IFDConnectResponse);
			QVERIFY(!connectResponse.getContextHandle().isEmpty());
			QCOMPARE(connectResponse.getSlotHandle(), QStringLiteral("test-reader"));
			QVERIFY(connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::IFDL_UnknownSlot);
		}


		void ifdConnectForReaderWithoutCardSendsAL_Unknown_Error()
		{
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);
			MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(IfdMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), IfdMessageType::IFDConnectResponse);
			QVERIFY(!connectResponse.getContextHandle().isEmpty());
			QCOMPARE(connectResponse.getSlotHandle(), QStringLiteral("test-reader"));
			QVERIFY(connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::AL_Unknown_Error);

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
		}


		void ifdConnectForReaderWithConnectedCardSendsIFDL_IFD_SharingViolation()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations
			reader->setCard(MockCardConfig());
			QTRY_COMPARE(sendSpy.count(), 2); // clazy:exclude=qstring-allocations

			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			ReaderInfo info = reader->getReaderInfo();
			info.setCardInfo(cardInfo);
			reader->setReaderInfo(info);
			QTRY_COMPARE(sendSpy.count(), 3); // clazy:exclude=qstring-allocations
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& connectResponse1Arguments = sendSpy.last();
			const QVariant connectResponse1Variant = connectResponse1Arguments.at(0);
			QVERIFY(connectResponse1Variant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse1(IfdMessage::parseByteArray(connectResponse1Variant.toByteArray()));
			QVERIFY(!connectResponse1.isIncomplete());
			QCOMPARE(connectResponse1.getType(), IfdMessageType::IFDConnectResponse);
			QVERIFY(!connectResponse1.getContextHandle().isEmpty());
			QVERIFY(!connectResponse1.getSlotHandle().isEmpty());

			QVERIFY(!connectResponse1.resultHasError());
			QCOMPARE(connectResponse1.getResultMinor(), ECardApiResult::Minor::null);

			sendSpy.clear();

			// Card connected, try to connect a second time and get an error.
			mDataChannel->onReceived(ifdConnectMsg);
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& connectResponse2Arguments = sendSpy.last();
			const QVariant connectResponse2Variant = connectResponse2Arguments.at(0);
			QVERIFY(connectResponse2Variant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse2(IfdMessage::parseByteArray(connectResponse2Variant.toByteArray()));
			QVERIFY(!connectResponse2.isIncomplete());
			QCOMPARE(connectResponse2.getType(), IfdMessageType::IFDConnectResponse);
			QVERIFY(!connectResponse2.getContextHandle().isEmpty());
			QCOMPARE(connectResponse2.getSlotHandle(), QStringLiteral("test-reader"));
			QVERIFY(connectResponse2.resultHasError());
			QCOMPARE(connectResponse2.getResultMinor(), ECardApiResult::Minor::IFDL_IFD_SharingViolation);

			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Card is already connected \"test-reader\"")));

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
		}


		void ifdDisconnectForReaderWithConnectedCardSendsCorrectResponse()
		{
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations
			reader->setCard(MockCardConfig());
			QTRY_COMPARE(sendSpy.count(), 2); // clazy:exclude=qstring-allocations
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			ReaderInfo info = reader->getReaderInfo();
			info.setCardInfo(cardInfo);
			reader->setReaderInfo(info);
			QTRY_COMPARE(sendSpy.count(), 3); // clazy:exclude=qstring-allocations
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(IfdMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), IfdMessageType::IFDConnectResponse);
			QCOMPARE(connectResponse.getContextHandle(), contextHandle);
			QVERIFY(!connectResponse.getSlotHandle().isEmpty());

			QVERIFY(!connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::null);

			sendSpy.clear();

			// Card connected, try to disconnect.
			const QByteArray ifdDisconnectMsg = IfdDisconnect(connectResponse.getSlotHandle()).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdDisconnectMsg);
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& disconnectResponseArguments = sendSpy.last();
			const QVariant disconnectResponseVariant = disconnectResponseArguments.at(0);
			QVERIFY(disconnectResponseVariant.canConvert<QByteArray>());

			const IfdDisconnectResponse disconnectResponse(IfdMessage::parseByteArray(disconnectResponseVariant.toByteArray()));
			QVERIFY(!disconnectResponse.isIncomplete());
			QCOMPARE(disconnectResponse.getType(), IfdMessageType::IFDDisconnectResponse);
			QCOMPARE(disconnectResponse.getContextHandle(), contextHandle);
			QCOMPARE(disconnectResponse.getSlotHandle(), connectResponse.getSlotHandle());
			QVERIFY(!disconnectResponse.resultHasError());
			QCOMPARE(disconnectResponse.getResultMinor(), ECardApiResult::Minor::null);

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
		}


		void ifdDisconnectForReaderWithWrongReaderNameSendsIFDL_InvalidSlotHandle()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations
			reader->setCard(MockCardConfig());
			QTRY_COMPARE(sendSpy.count(), 2); // clazy:exclude=qstring-allocations
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			ReaderInfo info = reader->getReaderInfo();
			info.setCardInfo(cardInfo);
			reader->setReaderInfo(info);
			QTRY_COMPARE(sendSpy.count(), 3); // clazy:exclude=qstring-allocations
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(IfdMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), IfdMessageType::IFDConnectResponse);
			QCOMPARE(connectResponse.getContextHandle(), contextHandle);
			QVERIFY(!connectResponse.getSlotHandle().isEmpty());

			QVERIFY(!connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::null);

			sendSpy.clear();

			// Card connected, try to disconnect from wrong reader.
			const QByteArray ifdDisconnectMsg = IfdDisconnect(QStringLiteral("wrong-reader")).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdDisconnectMsg);
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& disconnectResponseArguments = sendSpy.last();
			const QVariant disconnectResponseVariant = disconnectResponseArguments.at(0);
			QVERIFY(disconnectResponseVariant.canConvert<QByteArray>());

			const IfdDisconnectResponse disconnectResponse(IfdMessage::parseByteArray(disconnectResponseVariant.toByteArray()));
			QVERIFY(!disconnectResponse.isIncomplete());
			QCOMPARE(disconnectResponse.getType(), IfdMessageType::IFDDisconnectResponse);
			QCOMPARE(connectResponse.getContextHandle(), contextHandle);
			QCOMPARE(disconnectResponse.getSlotHandle(), QStringLiteral("wrong-reader"));
			QVERIFY(disconnectResponse.resultHasError());
			QCOMPARE(disconnectResponse.getResultMinor(), ECardApiResult::Minor::IFDL_InvalidSlotHandle);

			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Card is not connected \"wrong-reader\"")));

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
		}


		void ifdTransmitWithWrongReaderNameSendsIFDL_InvalidSlotHandle()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations
			reader->setCard(MockCardConfig());
			QTRY_COMPARE(sendSpy.count(), 2); // clazy:exclude=qstring-allocations
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			ReaderInfo info = reader->getReaderInfo();
			info.setCardInfo(cardInfo);
			reader->setReaderInfo(info);
			QTRY_COMPARE(sendSpy.count(), 3); // clazy:exclude=qstring-allocations
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(IfdMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), IfdMessageType::IFDConnectResponse);
			QCOMPARE(connectResponse.getContextHandle(), contextHandle);
			QVERIFY(!connectResponse.getSlotHandle().isEmpty());

			QVERIFY(!connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::null);

			sendSpy.clear();

			// Card connected, try to transmit to wrong reader.
			const QByteArray ifdTransmitMsg = IfdTransmit(QStringLiteral("wrong-reader"), QByteArray()).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdTransmitMsg);
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& transmitResponseArguments = sendSpy.last();
			const QVariant transmitResponseVariant = transmitResponseArguments.at(0);
			QVERIFY(transmitResponseVariant.canConvert<QByteArray>());

			const IfdTransmitResponse transmitResponse(IfdMessage::parseByteArray(transmitResponseVariant.toByteArray()));
			QVERIFY(!transmitResponse.isIncomplete());
			QCOMPARE(transmitResponse.getType(), IfdMessageType::IFDTransmitResponse);
			QCOMPARE(transmitResponse.getContextHandle(), contextHandle);
			QCOMPARE(transmitResponse.getSlotHandle(), QStringLiteral("wrong-reader"));
			QVERIFY(transmitResponse.resultHasError());
			QCOMPARE(transmitResponse.getResultMinor(), ECardApiResult::Minor::IFDL_InvalidSlotHandle);

			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Card is not connected \"wrong-reader\"")));

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
		}


		void ifdEstablishPACEChannelWithWrongReaderNameSendsIFDL_InvalidSlotHandle()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations
			reader->setCard(MockCardConfig());
			QTRY_COMPARE(sendSpy.count(), 2); // clazy:exclude=qstring-allocations
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			ReaderInfo info = reader->getReaderInfo();
			info.setCardInfo(cardInfo);
			reader->setReaderInfo(info);
			QTRY_COMPARE(sendSpy.count(), 3); // clazy:exclude=qstring-allocations
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(IfdMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), IfdMessageType::IFDConnectResponse);
			QCOMPARE(connectResponse.getContextHandle(), contextHandle);
			QVERIFY(!connectResponse.getSlotHandle().isEmpty());

			QVERIFY(!connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::null);

			sendSpy.clear();

			// Card connected, try to establish PACE with the wrong reader.
			EstablishPaceChannel establishPaceChannel(PacePasswordId::PACE_PIN);
			const QByteArray ifdEstablishPACEChannelMsg = IfdEstablishPaceChannel(QStringLiteral("wrong-reader"), establishPaceChannel, 6).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdEstablishPACEChannelMsg);
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& ifdEstablishPACEChannelResponseArguments = sendSpy.last();
			const QVariant ifdEstablishPACEChannelResponseVariant = ifdEstablishPACEChannelResponseArguments.at(0);
			QVERIFY(ifdEstablishPACEChannelResponseVariant.canConvert<QByteArray>());

			const IfdEstablishPaceChannelResponse ifdEstablishPACEChannelResponse(IfdMessage::parseByteArray(ifdEstablishPACEChannelResponseVariant.toByteArray()));
			QVERIFY(!ifdEstablishPACEChannelResponse.isIncomplete());
			QCOMPARE(ifdEstablishPACEChannelResponse.getType(), IfdMessageType::IFDEstablishPACEChannelResponse);
			QCOMPARE(ifdEstablishPACEChannelResponse.getContextHandle(), contextHandle);
			QCOMPARE(ifdEstablishPACEChannelResponse.getSlotHandle(), QStringLiteral("wrong-reader"));
			QVERIFY(ifdEstablishPACEChannelResponse.resultHasError());
			QCOMPARE(ifdEstablishPACEChannelResponse.getResultMinor(), ECardApiResult::Minor::IFDL_InvalidSlotHandle);

			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Card is not connected \"wrong-reader\"")));

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
		}


		void ifdTransmit_data()
		{
			QTest::addColumn<bool>("sendProgressMessage");

			QTest::newRow("Send message") << true;
			QTest::newRow("Don't send message") << false;
		}


		void ifdTransmit()
		{
			QFETCH(bool, sendProgressMessage);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations
			reader->setCard(MockCardConfig({
						{CardReturnCode::OK, QByteArray("9000")}
					}));
			QTRY_COMPARE(sendSpy.count(), 2); // clazy:exclude=qstring-allocations
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			ReaderInfo info = reader->getReaderInfo();
			info.setCardInfo(cardInfo);
			reader->setReaderInfo(info);
			QTRY_COMPARE(sendSpy.count(), 3); // clazy:exclude=qstring-allocations
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(IfdMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), IfdMessageType::IFDConnectResponse);
			QCOMPARE(connectResponse.getContextHandle(), contextHandle);
			QVERIFY(!connectResponse.getSlotHandle().isEmpty());

			QVERIFY(!connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::null);

			sendSpy.clear();

			QSignalSpy displayTextSpy(&serverMessageHandler, &ServerMessageHandler::fireDisplayTextChanged);
			// Card connected, try to provoke message.
			const QByteArray ifdTransmitMsg = IfdTransmit(connectResponse.getSlotHandle(), QByteArray("ABCDEF"), sendProgressMessage ? QStringLiteral("DummyText") : QString()).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdTransmitMsg);
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& transmitResponseArguments = sendSpy.last();
			const QVariant transmitResponseVariant = transmitResponseArguments.at(0);
			QVERIFY(transmitResponseVariant.canConvert<QByteArray>());

			const IfdTransmitResponse transmitResponse(IfdMessage::parseByteArray(transmitResponseVariant.toByteArray()));
			QVERIFY(!transmitResponse.isIncomplete());
			QCOMPARE(transmitResponse.getType(), IfdMessageType::IFDTransmitResponse);
			QCOMPARE(transmitResponse.getContextHandle(), contextHandle);
			QCOMPARE(transmitResponse.getSlotHandle(), connectResponse.getSlotHandle());
			QVERIFY(!transmitResponse.resultHasError());
			QCOMPARE(transmitResponse.getResultMinor(), ECardApiResult::Minor::null);
			QCOMPARE(displayTextSpy.count(), sendProgressMessage ? 1 : 0);

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
		}


		void ifdEstablishPACEChannelWithBasicReaderNameSendsAL_Unknown_Error()
		{
			const bool pinpadModeToSave = Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(false);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations
			reader->setCard(MockCardConfig());
			QTRY_COMPARE(sendSpy.count(), 2); // clazy:exclude=qstring-allocations
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			ReaderInfo info = reader->getReaderInfo();
			info.setCardInfo(cardInfo);
			reader->setReaderInfo(info);
			QTRY_COMPARE(sendSpy.count(), 3); // clazy:exclude=qstring-allocations
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(IfdMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), IfdMessageType::IFDConnectResponse);
			QCOMPARE(connectResponse.getContextHandle(), contextHandle);
			QVERIFY(!connectResponse.getSlotHandle().isEmpty());

			QVERIFY(!connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::null);

			sendSpy.clear();

			// Card connected, try to establish PACE with basic reader while not in pinpad mode.
			const QByteArray ifdEstablishPACEChannelMsg = IfdEstablishPaceChannel(connectResponse.getSlotHandle(), EstablishPaceChannel(), 6).toByteArray(IfdVersion::Version::latest, contextHandle);
			mDataChannel->onReceived(ifdEstablishPACEChannelMsg);
			QTRY_COMPARE(sendSpy.count(), 1); // clazy:exclude=qstring-allocations

			const QList<QVariant>& ifdEstablishPACEChannelResponseArguments = sendSpy.last();
			const QVariant ifdEstablishPACEChannelResponseVariant = ifdEstablishPACEChannelResponseArguments.at(0);
			QVERIFY(ifdEstablishPACEChannelResponseVariant.canConvert<QByteArray>());

			const IfdEstablishPaceChannelResponse ifdEstablishPACEChannelResponse(IfdMessage::parseByteArray(ifdEstablishPACEChannelResponseVariant.toByteArray()));
			QVERIFY(!ifdEstablishPACEChannelResponse.isIncomplete());
			QCOMPARE(ifdEstablishPACEChannelResponse.getType(), IfdMessageType::IFDEstablishPACEChannelResponse);
			QCOMPARE(ifdEstablishPACEChannelResponse.getContextHandle(), contextHandle);
			QCOMPARE(ifdEstablishPACEChannelResponse.getSlotHandle(), connectResponse.getSlotHandle());
			QVERIFY(ifdEstablishPACEChannelResponse.resultHasError());
			QCOMPARE(ifdEstablishPACEChannelResponse.getResultMinor(), ECardApiResult::Minor::AL_Unknown_Error);

			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("EstablishPaceChannel is only available in pin pad mode.")));

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(pinpadModeToSave);
		}


		void test_handleIfdModifyPin()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"InputData\": \"abcd1234\",\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDModifyPIN\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			ServerMessageHandlerImpl serverMsgHandler(mDataChannel);
			QSignalSpy spyModifyPin(&serverMsgHandler, &ServerMessageHandler::fireModifyPin);
			QString contextHandle;
			ensureContext(contextHandle);

			QVERIFY(!TestFileHelper::containsLog(logSpy, QLatin1String("ModifyPin is only available in pin pad mode.")));
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(false);
			Q_EMIT mDispatcher->fireReceived(IfdMessageType::IFDModifyPIN, obj, QString());
			QCOMPARE(mDispatcher->getMessage()->getType(), IfdMessageType::IFDModifyPINResponse);
			const auto& msg1 = mDispatcher->getMessage().staticCast<const IfdModifyPinResponse>();
			QCOMPARE(msg1->getResultMinor(), ECardApiResult::Minor::AL_Unknown_Error);
			QCOMPARE(msg1->getSlotHandle(), "SlotHandle");
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("ModifyPin is only available in pin pad mode.")));

			QVERIFY(!TestFileHelper::containsLog(logSpy, QLatin1String("Card is not connected")));
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(true);
			Q_EMIT mDispatcher->fireReceived(IfdMessageType::IFDModifyPIN, obj, QString());
			const auto& msg2 = mDispatcher->getMessage().staticCast<const IfdModifyPinResponse>();
			QCOMPARE(msg2->getResultMinor(), ECardApiResult::Minor::IFDL_InvalidSlotHandle);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Card is not connected")));
		}


		void test_SendModifyPinResponse_data()
		{
			QTest::addColumn<QByteArray>("statusCode");
			QTest::addColumn<ECardApiResult::Minor>("minor");

			QTest::newRow("success") << QByteArray::fromHex("9000") << ECardApiResult::Minor::null;
			QTest::newRow("empty") << QByteArray() << ECardApiResult::Minor::IFDL_Terminal_NoCard;
			QTest::newRow("inputTimeout") << QByteArray::fromHex("6400") << ECardApiResult::Minor::IFDL_Timeout_Error;
			QTest::newRow("inputCancelled") << QByteArray::fromHex("6401") << ECardApiResult::Minor::IFDL_CancellationByUser;
			QTest::newRow("passwordsDiffer") << QByteArray::fromHex("6402") << ECardApiResult::Minor::IFDL_IO_RepeatedDataMismatch;
			QTest::newRow("passwordOutOfRange") << QByteArray::fromHex("6403") << ECardApiResult::Minor::IFDL_IO_UnknownPINFormat;
			QTest::newRow("default") << QByteArray::fromHex("0001") << ECardApiResult::Minor::AL_Unknown_Error;
		}


		void test_SendModifyPinResponse()
		{
			QFETCH(QByteArray, statusCode);
			QFETCH(ECardApiResult::Minor, minor);

			ServerMessageHandlerImpl serverMsgHandler(mDataChannel);
			const QString slotHandle("Slot Handle");
			const ResponseApdu apdu(statusCode);

			QString contextHandle;
			ensureContext(contextHandle);
			serverMsgHandler.sendModifyPinResponse(slotHandle, apdu);
			QCOMPARE(mDispatcher->getMessage()->getType(), IfdMessageType::IFDModifyPINResponse);
			const auto& msg = mDispatcher->getMessage().staticCast<const IfdModifyPinResponse>();
			QCOMPARE(msg->getResultMinor(), minor);
			QCOMPARE(msg->getSlotHandle(), slotHandle);
		}


		void test_EstablishPaceChannelResponse_data()
		{
			QTest::addColumn<CardReturnCode>("returnCode");
			QTest::addColumn<ECardApiResult::Minor>("minor");

			QTest::newRow("unknown") << CardReturnCode::UNKNOWN << ECardApiResult::Minor::AL_Unknown_Error;
			QTest::newRow("unknown") << CardReturnCode::CARD_NOT_FOUND << ECardApiResult::Minor::IFDL_Terminal_NoCard;
			QTest::newRow("default") << CardReturnCode::OK << ECardApiResult::Minor::null;
		}


		void test_EstablishPaceChannelResponse()
		{
			QFETCH(CardReturnCode, returnCode);
			QFETCH(ECardApiResult::Minor, minor);

			const QString slotHandle("Slot Handle");
			EstablishPaceChannelOutput output;
			output.setPaceReturnCode(returnCode);

			ServerMessageHandlerImpl serverMsgHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			serverMsgHandler.sendEstablishPaceChannelResponse(slotHandle, output);
			QCOMPARE(mDispatcher->getMessage()->getType(), IfdMessageType::IFDEstablishPACEChannelResponse);
			const auto& msg = mDispatcher->getMessage().staticCast<const IfdEstablishPaceChannelResponse>();
			QCOMPARE(msg->getResultMinor(), minor);
			QCOMPARE(msg->getSlotHandle(), slotHandle);
		}


};


QTEST_GUILESS_MAIN(test_ServerMessageHandler)
#include "test_ServerMessageHandler.moc"
