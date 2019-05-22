/*!
 * \brief Unit tests for \ref ServerMessageHandlerImpl
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ServerMessageHandler.h"

#include "AppSettings.h"
#include "LogHandler.h"
#include "messages/IfdConnect.h"
#include "messages/IfdConnectResponse.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdDisconnectResponse.h"
#include "messages/IfdError.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdEstablishPaceChannelResponse.h"
#include "messages/IfdStatus.h"
#include "messages/IfdTransmit.h"
#include "messages/IfdTransmitResponse.h"

#include "MockDataChannel.h"
#include "MockReaderManagerPlugIn.h"
#include "TestFileHelper.h"

#include <QSignalSpy>
#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)


using namespace governikus;


class test_ServerMessageHandler
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<MockDataChannel> mDataChannel;


		void removeReaderAndConsumeMessages(const QString& pReaderName)
		{
			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);
			MockReaderManagerPlugIn::getInstance().removeReader(pReaderName);
			QTRY_COMPARE(sendSpy.count(), 1);
		}


		void ensureContext(QString& pContextHandle)
		{
			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			const QByteArray establishContextMsg("{\n"
												 "    \"msg\": \"IFDEstablishContext\",\n"
												 "    \"Protocol\": \"IFDInterface_WebSocket_v0\",\n"
												 "    \"UDName\": \"MAC-MINI\"\n"
												 "}");

			mDataChannel->onReceived(establishContextMsg);

			QTRY_COMPARE(sendSpy.count(), 1);
			const QList<QVariant>& establishContextResponseArguments = sendSpy.last();

			const QVariant establishContextResponseVariant = establishContextResponseArguments.at(0);
			QVERIFY(establishContextResponseVariant.canConvert<QByteArray>());
			const IfdEstablishContextResponse establishContextResponse(RemoteMessage::parseByteArray(establishContextResponseVariant.toByteArray()));
			QVERIFY(!establishContextResponse.isIncomplete());
			QCOMPARE(establishContextResponse.getType(), RemoteCardMessageType::IFDEstablishContextResponse);

			pContextHandle = establishContextResponse.getContextHandle();
			QVERIFY(!pContextHandle.isEmpty());
		}


	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->init();
			readerManager->getPlugInInfos(); // just to wait until initialization finished
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
			QSignalSpy spyLog(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			IfdConnectResponse unexpectedMsg(QStringLiteral("RemoteReader"));

			mDataChannel->onReceived(unexpectedMsg.toByteArray(QStringLiteral("invalidConextHandle")));
			QVERIFY(TestFileHelper::containsLog(spyLog, QLatin1String("Invalid context handle received")));
		}


		void checkLogOnUnexpectedMessageWithContext()
		{
			QSignalSpy spyLog(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			QSignalSpy spyContextHandle(mDataChannel.data(), &MockDataChannel::fireSend);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);

			IfdEstablishContext establishContext(QStringLiteral("IFDInterface_WebSocket_v0"), DeviceInfo::getName());
			mDataChannel->onReceived(establishContext.toByteArray(QString()));

			const QJsonDocument& doc = QJsonDocument::fromJson(spyContextHandle.at(0).at(0).toByteArray());
			const QString& contextHandle = doc.object().value(QLatin1String("ContextHandle")).toString();

			IfdConnectResponse unexpectedMsg(QStringLiteral("RemoteReader"));
			mDataChannel->onReceived(unexpectedMsg.toByteArray(contextHandle));
			QVERIFY(TestFileHelper::containsLog(spyLog, QLatin1String("Received an unexpected message of type: IFDConnectResponse")));
		}


		void checkLogOnInvalidMessage()
		{
			QSignalSpy spyLog(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);

			mDataChannel->onReceived("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"msg\": \"RANDOM_STUFF\"\n"
									 "}\n");
			QVERIFY(TestFileHelper::containsLog(spyLog, QLatin1String("Invalid messageType received")));
		}


		void testUnexpectedMessagesCauseAnIfdErrorMessage()
		{
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			// We have a context handle: send unexpected messages and verify that an error message is sent back.
			sendSpy.clear();

			const QByteArrayList serverMessages({
						IfdStatus("NFC Reader", PaceCapabilities(), 500, true).toByteArray(contextHandle),
						IfdConnectResponse("NFC Reader").toByteArray(contextHandle),
						IfdDisconnectResponse("NFC Reader").toByteArray(contextHandle),
						IfdTransmitResponse("NFC Reader", "9000").toByteArray(contextHandle),
						IfdEstablishPaceChannelResponse("My little Reader", "abcd1234").toByteArray(contextHandle)
					});
			for (const auto& serverMessage : serverMessages)
			{
				mDataChannel->onReceived(serverMessage);
				QTRY_COMPARE(sendSpy.count(), 1);

				const QList<QVariant>& errorMessageArguments = sendSpy.last();
				const QVariant errorMessageVariant = errorMessageArguments.at(0);
				QVERIFY(errorMessageVariant.canConvert<QByteArray>());

				const IfdError errorMessage(RemoteMessage::parseByteArray(errorMessageVariant.toByteArray()));
				QVERIFY(!errorMessage.isIncomplete());
				QCOMPARE(errorMessage.getType(), RemoteCardMessageType::IFDError);
				QCOMPARE(errorMessage.getContextHandle(), contextHandle);
				QCOMPARE(errorMessage.getSlotHandle(), QString());
				QVERIFY(errorMessage.resultHasError());
				QCOMPARE(errorMessage.getResultMinor(), ECardApiResult::Minor::AL_Unkown_API_Function);

				sendSpy.clear();
			}
		}


		void ifdConnectForUnconnectedReaderSendsIFDL_UnknownSlot()
		{
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);
			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(RemoteMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), RemoteCardMessageType::IFDConnectResponse);
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
			QTRY_COMPARE(sendSpy.count(), 1);
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(RemoteMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), RemoteCardMessageType::IFDConnectResponse);
			QVERIFY(!connectResponse.getContextHandle().isEmpty());
			QCOMPARE(connectResponse.getSlotHandle(), QStringLiteral("test-reader"));
			QVERIFY(connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::AL_Unknown_Error);

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
		}


		void ifdConnectForReaderWithConnectedCardSendsIFDL_IFD_SharingViolation()
		{
			QSignalSpy spyLog(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			reader->setCard(MockCardConfig());
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			reader->getReaderInfo().setCardInfo(cardInfo);
			QTRY_COMPARE(sendSpy.count(), 1);
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& connectResponse1Arguments = sendSpy.last();
			const QVariant connectResponse1Variant = connectResponse1Arguments.at(0);
			QVERIFY(connectResponse1Variant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse1(RemoteMessage::parseByteArray(connectResponse1Variant.toByteArray()));
			QVERIFY(!connectResponse1.isIncomplete());
			QCOMPARE(connectResponse1.getType(), RemoteCardMessageType::IFDConnectResponse);
			QVERIFY(!connectResponse1.getContextHandle().isEmpty());
			QVERIFY(!connectResponse1.getSlotHandle().isEmpty());

			QVERIFY(!connectResponse1.resultHasError());
			QCOMPARE(connectResponse1.getResultMinor(), ECardApiResult::Minor::null);

			sendSpy.clear();

			// Card connected, try to connect a second time and get an error.
			mDataChannel->onReceived(ifdConnectMsg);
			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& connectResponse2Arguments = sendSpy.last();
			const QVariant connectResponse2Variant = connectResponse2Arguments.at(0);
			QVERIFY(connectResponse2Variant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse2(RemoteMessage::parseByteArray(connectResponse2Variant.toByteArray()));
			QVERIFY(!connectResponse2.isIncomplete());
			QCOMPARE(connectResponse2.getType(), RemoteCardMessageType::IFDConnectResponse);
			QVERIFY(!connectResponse2.getContextHandle().isEmpty());
			QCOMPARE(connectResponse2.getSlotHandle(), QStringLiteral("test-reader"));
			QVERIFY(connectResponse2.resultHasError());
			QCOMPARE(connectResponse2.getResultMinor(), ECardApiResult::Minor::IFDL_IFD_SharingViolation);

			QVERIFY(TestFileHelper::containsLog(spyLog, QLatin1String("Card is already connected \"test-reader\"")));

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
		}


		void ifdDisconnectForReaderWithConnectedCardSendsCorrectResponse()
		{
			QSignalSpy spyLog(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			reader->setCard(MockCardConfig());
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			reader->getReaderInfo().setCardInfo(cardInfo);
			QTRY_COMPARE(sendSpy.count(), 1);
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(RemoteMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), RemoteCardMessageType::IFDConnectResponse);
			QCOMPARE(connectResponse.getContextHandle(), contextHandle);
			QVERIFY(!connectResponse.getSlotHandle().isEmpty());

			QVERIFY(!connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::null);

			sendSpy.clear();

			// Card connected, try to disconnect.
			const QByteArray ifdDisconnectMsg = IfdDisconnect(QStringLiteral("test-reader")).toByteArray(contextHandle);
			mDataChannel->onReceived(ifdDisconnectMsg);
			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& disconnectResponseArguments = sendSpy.last();
			const QVariant disconnectResponseVariant = disconnectResponseArguments.at(0);
			QVERIFY(disconnectResponseVariant.canConvert<QByteArray>());

			const IfdDisconnectResponse disconnectResponse(RemoteMessage::parseByteArray(disconnectResponseVariant.toByteArray()));
			QVERIFY(!disconnectResponse.isIncomplete());
			QCOMPARE(disconnectResponse.getType(), RemoteCardMessageType::IFDDisconnectResponse);
			QCOMPARE(disconnectResponse.getContextHandle(), contextHandle);
			QCOMPARE(disconnectResponse.getSlotHandle(), connectResponse.getSlotHandle());
			QVERIFY(!disconnectResponse.resultHasError());
			QCOMPARE(disconnectResponse.getResultMinor(), ECardApiResult::Minor::null);

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
		}


		void ifdDisconnectForReaderWithWrongReaderNameSendsIFDL_InvalidSlotHandle()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			reader->setCard(MockCardConfig());
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			reader->getReaderInfo().setCardInfo(cardInfo);
			QTRY_COMPARE(sendSpy.count(), 1);
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(RemoteMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), RemoteCardMessageType::IFDConnectResponse);
			QCOMPARE(connectResponse.getContextHandle(), contextHandle);
			QVERIFY(!connectResponse.getSlotHandle().isEmpty());

			QVERIFY(!connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::null);

			sendSpy.clear();

			// Card connected, try to disconnect from wrong reader.
			const QByteArray ifdDisconnectMsg = IfdDisconnect(QStringLiteral("wrong-reader")).toByteArray(contextHandle);
			mDataChannel->onReceived(ifdDisconnectMsg);
			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& disconnectResponseArguments = sendSpy.last();
			const QVariant disconnectResponseVariant = disconnectResponseArguments.at(0);
			QVERIFY(disconnectResponseVariant.canConvert<QByteArray>());

			const IfdDisconnectResponse disconnectResponse(RemoteMessage::parseByteArray(disconnectResponseVariant.toByteArray()));
			QVERIFY(!disconnectResponse.isIncomplete());
			QCOMPARE(disconnectResponse.getType(), RemoteCardMessageType::IFDDisconnectResponse);
			QCOMPARE(connectResponse.getContextHandle(), contextHandle);
			QCOMPARE(disconnectResponse.getSlotHandle(), QStringLiteral("wrong-reader"));
			QVERIFY(disconnectResponse.resultHasError());
			QCOMPARE(disconnectResponse.getResultMinor(), ECardApiResult::Minor::IFDL_InvalidSlotHandle);

			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Card is not connected \"wrong-reader\"")));

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
		}


		void ifdTransmitWithWrongReaderNameSendsIFDL_InvalidSlotHandle()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			reader->setCard(MockCardConfig());
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			reader->getReaderInfo().setCardInfo(cardInfo);
			QTRY_COMPARE(sendSpy.count(), 1);
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(RemoteMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), RemoteCardMessageType::IFDConnectResponse);
			QCOMPARE(connectResponse.getContextHandle(), contextHandle);
			QVERIFY(!connectResponse.getSlotHandle().isEmpty());

			QVERIFY(!connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::null);

			sendSpy.clear();

			// Card connected, try to transmit to wrong reader.
			const QByteArray ifdTransmitMsg = IfdTransmit(QStringLiteral("wrong-reader"), QByteArray()).toByteArray(contextHandle);
			mDataChannel->onReceived(ifdTransmitMsg);
			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& transmitResponseArguments = sendSpy.last();
			const QVariant transmitResponseVariant = transmitResponseArguments.at(0);
			QVERIFY(transmitResponseVariant.canConvert<QByteArray>());

			const IfdTransmitResponse transmitResponse(RemoteMessage::parseByteArray(transmitResponseVariant.toByteArray()));
			QVERIFY(!transmitResponse.isIncomplete());
			QCOMPARE(transmitResponse.getType(), RemoteCardMessageType::IFDTransmitResponse);
			QCOMPARE(transmitResponse.getContextHandle(), contextHandle);
			QCOMPARE(transmitResponse.getSlotHandle(), QStringLiteral("wrong-reader"));
			QVERIFY(transmitResponse.resultHasError());
			QCOMPARE(transmitResponse.getResultMinor(), ECardApiResult::Minor::IFDL_InvalidSlotHandle);

			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Card is not connected \"wrong-reader\"")));

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
		}


		void ifdEstablishPACEChannelWithWrongReaderNameSendsIFDL_InvalidSlotHandle()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			reader->setCard(MockCardConfig());
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			reader->getReaderInfo().setCardInfo(cardInfo);
			QTRY_COMPARE(sendSpy.count(), 1);
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(RemoteMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), RemoteCardMessageType::IFDConnectResponse);
			QCOMPARE(connectResponse.getContextHandle(), contextHandle);
			QVERIFY(!connectResponse.getSlotHandle().isEmpty());

			QVERIFY(!connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::null);

			sendSpy.clear();

			// Card connected, try to establish PACE with the wrong reader.
			const QByteArray ifdEstablishPACEChannelMsg = IfdEstablishPaceChannel(QStringLiteral("wrong-reader"), QByteArray()).toByteArray(contextHandle);
			mDataChannel->onReceived(ifdEstablishPACEChannelMsg);
			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& ifdEstablishPACEChannelResponseArguments = sendSpy.last();
			const QVariant ifdEstablishPACEChannelResponseVariant = ifdEstablishPACEChannelResponseArguments.at(0);
			QVERIFY(ifdEstablishPACEChannelResponseVariant.canConvert<QByteArray>());

			const IfdEstablishPaceChannelResponse ifdEstablishPACEChannelResponse(RemoteMessage::parseByteArray(ifdEstablishPACEChannelResponseVariant.toByteArray()));
			QVERIFY(!ifdEstablishPACEChannelResponse.isIncomplete());
			QCOMPARE(ifdEstablishPACEChannelResponse.getType(), RemoteCardMessageType::IFDEstablishPACEChannelResponse);
			QCOMPARE(ifdEstablishPACEChannelResponse.getContextHandle(), contextHandle);
			QCOMPARE(ifdEstablishPACEChannelResponse.getSlotHandle(), QStringLiteral("wrong-reader"));
			QVERIFY(ifdEstablishPACEChannelResponse.resultHasError());
			QCOMPARE(ifdEstablishPACEChannelResponse.getResultMinor(), ECardApiResult::Minor::IFDL_InvalidSlotHandle);

			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Card is not connected \"wrong-reader\"")));

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
		}


		void ifdEstablishPACEChannelWithBasicReaderNameSendsAL_Unknown_Error()
		{
			const bool pinpadModeToSave = Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(false);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			QString contextHandle;
			ensureContext(contextHandle);

			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("test-reader");
			reader->setCard(MockCardConfig());
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			reader->getReaderInfo().setCardInfo(cardInfo);
			QTRY_COMPARE(sendSpy.count(), 1);
			sendSpy.clear();

			const QByteArray ifdConnectMsg = IfdConnect(QStringLiteral("test-reader"), true).toByteArray(contextHandle);
			mDataChannel->onReceived(ifdConnectMsg);

			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& connectResponseArguments = sendSpy.last();
			const QVariant connectResponseVariant = connectResponseArguments.at(0);
			QVERIFY(connectResponseVariant.canConvert<QByteArray>());

			const IfdConnectResponse connectResponse(RemoteMessage::parseByteArray(connectResponseVariant.toByteArray()));
			QVERIFY(!connectResponse.isIncomplete());
			QCOMPARE(connectResponse.getType(), RemoteCardMessageType::IFDConnectResponse);
			QCOMPARE(connectResponse.getContextHandle(), contextHandle);
			QVERIFY(!connectResponse.getSlotHandle().isEmpty());

			QVERIFY(!connectResponse.resultHasError());
			QCOMPARE(connectResponse.getResultMinor(), ECardApiResult::Minor::null);

			sendSpy.clear();

			// Card connected, try to establish PACE with basic reader while not in pinpad mode.
			const QByteArray ifdEstablishPACEChannelMsg = IfdEstablishPaceChannel(QStringLiteral("test-reader"), QByteArray()).toByteArray(contextHandle);
			mDataChannel->onReceived(ifdEstablishPACEChannelMsg);
			QTRY_COMPARE(sendSpy.count(), 1);

			const QList<QVariant>& ifdEstablishPACEChannelResponseArguments = sendSpy.last();
			const QVariant ifdEstablishPACEChannelResponseVariant = ifdEstablishPACEChannelResponseArguments.at(0);
			QVERIFY(ifdEstablishPACEChannelResponseVariant.canConvert<QByteArray>());

			const IfdEstablishPaceChannelResponse ifdEstablishPACEChannelResponse(RemoteMessage::parseByteArray(ifdEstablishPACEChannelResponseVariant.toByteArray()));
			QVERIFY(!ifdEstablishPACEChannelResponse.isIncomplete());
			QCOMPARE(ifdEstablishPACEChannelResponse.getType(), RemoteCardMessageType::IFDEstablishPACEChannelResponse);
			QCOMPARE(ifdEstablishPACEChannelResponse.getContextHandle(), contextHandle);
			QCOMPARE(ifdEstablishPACEChannelResponse.getSlotHandle(), connectResponse.getSlotHandle());
			QVERIFY(ifdEstablishPACEChannelResponse.resultHasError());
			QCOMPARE(ifdEstablishPACEChannelResponse.getResultMinor(), ECardApiResult::Minor::AL_Unknown_Error);

			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("EstablishPaceChannel is only available in pin pad mode.")));

			removeReaderAndConsumeMessages(QStringLiteral("test-reader"));
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(pinpadModeToSave);
		}


};


QTEST_GUILESS_MAIN(test_ServerMessageHandler)
#include "test_ServerMessageHandler.moc"
