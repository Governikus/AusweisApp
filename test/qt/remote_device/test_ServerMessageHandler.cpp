/*!
 * \brief Unit tests for \ref test_ServerMessageHandlerImpl
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ServerMessageHandler.h"

#include "LogHandler.h"
#include "messages/IfdConnectResponse.h"
#include "messages/IfdError.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"
#include "messages/RemoteMessageParser.h"
#include "MockDataChannel.h"
#include "TestFileHelper.h"

#include <QSignalSpy>
#include <QtTest>


using namespace governikus;


class test_ServerMessageHandler
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<MockDataChannel> mDataChannel;
		const RemoteMessageParser mParser;

		QVector<QByteArray> makeServerMessages(const QByteArray& pContextHandle)
		{
			QVector<QByteArray> serverMessages;

			auto m = [&](const char* pJsonByteData) -> QByteArray
					{
						QByteArray result(pJsonByteData);
						result.replace(QByteArray("contextHandleToReplace"), pContextHandle);

						return result;
					};

			serverMessages
				<< m("{\n"
					 "    \"ContextHandle\": \"contextHandleToReplace\",\n"
					 "    \"msg\": \"IFDStatus\",\n"
					 "    \"ConnectedReader\": true,\n"
					 "    \"IFDName\": \"Remote Reader\",\n"
					 "    \"SlotName\": \"NFC Reader\",\n"
					 "    \"PINCapabilities\": {\n"
					 "        \"Destroy\": false,\n"
					 "        \"PACE\": false,\n"
					 "        \"eID\": false,\n"
					 "        \"eSign\": false\n"
					 "    },\n"
					 "    \"MaxAPDULength\": 500,\n"
					 "    \"CardAvailable\": false\n"
					 "}")
				<<
				m("{\n"
				  "    \"ContextHandle\": \"contextHandleToReplace\",\n"
				  "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor/resultmajor#error\",\n"
				  "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\",\n"
				  "    \"msg\": \"IFDConnectResponse\",\n"
				  "    \"SlotHandle\": \"NFC Reader\"\n"
				  "}\n")
				<<
				m("{\n"
				  "    \"ContextHandle\": \"contextHandleToReplace\",\n"
				  "    \"SlotHandle\": \"NFC Reader\",\n"
				  "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor/resultmajor#error\",\n"
				  "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\",\n"
				  "    \"msg\": \"IFDDisconnectResponse\"\n"
				  "}\n")
				<<
				m("{\n"
				  "    \"ContextHandle\": \"contextHandleToReplace\",\n"
				  "    \"SlotHandle\": \"NFC Reader\",\n"
				  "    \"ResponseAPDUs\": [\n"
				  "		\"9000\"\n"
				  "    ],\n"
				  "    \"msg\": \"IFDTransmitResponse\",\n"
				  "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor/resultmajor#error\",\n"
				  "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\"\n"
				  "}")
				<<
				m("{\n"
				  "    \"ContextHandle\": \"contextHandleToReplace\",\n"
				  "    \"OutputData\": \"abcd1234\",\n"
				  "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor/resultmajor#ok\",\n"
				  "    \"ResultMinor\": null,\n"
				  "    \"SlotHandle\": \"My little Reader\",\n"
				  "    \"msg\": \"IFDEstablishPACEChannelResponse\"\n"
				  "}\n");

			return serverMessages;
		}


		void waitForSignals(QSignalSpy* const pSpy, const int pExpectedCount, const int pTimeoutMs)
		{
			for (int tryCount = 0; pSpy->count() < pExpectedCount && tryCount < pExpectedCount; ++tryCount)
			{
				pSpy->wait(pTimeoutMs);
			}
			QCOMPARE(pSpy->count(), pExpectedCount);
		}


	private Q_SLOTS:
		void initTestCase()
		{
			LogHandler::getInstance().init();

		}


		void init()
		{
			mDataChannel.reset(new MockDataChannel());
		}


		void cleanup()
		{
			LogHandler::getInstance().resetBacklog();
		}


		void checkLogOnInvalidContext()
		{
			QSignalSpy spyLog(&LogHandler::getInstance(), &LogHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);
			IfdConnectResponse unexpectedMsg(QStringLiteral("RemoteReader"));

			mDataChannel->onReceived(unexpectedMsg.toJson(QStringLiteral("invalidConextHandle")).toJson());
			QVERIFY(TestFileHelper::containsLog(spyLog, QLatin1String("Invalid context handle received")));
		}


		void checkLogOnUnexpectedMessageWithContext()
		{
			QSignalSpy spyLog(&LogHandler::getInstance(), &LogHandler::fireLog);
			QSignalSpy spyContextHandle(mDataChannel.data(), &MockDataChannel::fireSend);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);

			IfdEstablishContext establishContext(QStringLiteral("IFDInterface_WebSocket_v0"), DeviceInfo::getName());
			mDataChannel->onReceived(establishContext.toJson(QString()).toJson());

			const QJsonDocument& doc = QJsonDocument::fromJson(spyContextHandle.at(0).at(0).toByteArray());
			const QString& contextHandle = doc.object().value(QLatin1String("ContextHandle")).toString();

			IfdConnectResponse unexpectedMsg(QStringLiteral("RemoteReader"));
			mDataChannel->onReceived(unexpectedMsg.toJson(contextHandle).toJson());
			QVERIFY(TestFileHelper::containsLog(spyLog, QLatin1String("Received an unexpected message of type: IFDConnectResponse")));
		}


		void checkLogOnInvalidMessage()
		{
			QSignalSpy spyLog(&LogHandler::getInstance(), &LogHandler::fireLog);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);

			mDataChannel->onReceived("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"msg\": \"RANDOM_STUFF\"\n"
									 "}\n");
			QVERIFY(TestFileHelper::containsLog(spyLog, QLatin1String("Invalid message received")));
		}


		void testUnexpectedMessagesCauseAnIfdErrorMessage()
		{
			QSignalSpy sendSpy(mDataChannel.data(), &MockDataChannel::fireSend);
			ServerMessageHandlerImpl serverMessageHandler(mDataChannel);

			const QByteArray establishContextMsg("{\n"
												 "    \"msg\": \"IFDEstablishContext\",\n"
												 "    \"Protocol\": \"IFDInterface_WebSocket_v0\",\n"
												 "	\"UDName\": \"MAC-MINI\"\n"
												 "}");

			mDataChannel->onReceived(establishContextMsg);

			waitForSignals(&sendSpy, 1, 1000);
			const QList<QVariant>& establishContextResponseArguments = sendSpy.last();

			const QVariant establishContextResponseVariant = establishContextResponseArguments.at(0);
			QVERIFY(establishContextResponseVariant.canConvert<QByteArray>());
			const IfdEstablishContextResponse establishContextResponse(RemoteMessage::parseByteArray(establishContextResponseVariant.toByteArray()));
			QVERIFY(establishContextResponse.isValid());
			QCOMPARE(establishContextResponse.getType(), RemoteCardMessageType::IFDEstablishContextResponse);

			const QByteArray contextHandle = establishContextResponse.getContextHandle().toUtf8();
			QVERIFY(!contextHandle.isEmpty());\

			// We have a context handle: send unexpected messages and verify that an error message is sent back.
			sendSpy.clear();
			const QVector<QByteArray> serverMessages = makeServerMessages(contextHandle);
			for (const QByteArray& serverMessage : serverMessages)
			{
				mDataChannel->onReceived(serverMessage);
				waitForSignals(&sendSpy, 1, 1000);

				const QList<QVariant>& errorMessageArguments = sendSpy.last();
				const QVariant errorMessageVariant = errorMessageArguments.at(0);
				QVERIFY(errorMessageVariant.canConvert<QByteArray>());

				const IfdError errorMessage(RemoteMessage::parseByteArray(errorMessageVariant.toByteArray()));
				QVERIFY(errorMessage.isValid());
				QCOMPARE(errorMessage.getType(), RemoteCardMessageType::IFDError);
				QCOMPARE(errorMessage.getContextHandle(), QString::fromUtf8(contextHandle));
				QCOMPARE(errorMessage.getSlotHandle(), QString());
				QVERIFY(errorMessage.resultHasError());
				QCOMPARE(errorMessage.getResultMinor(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#unknownAPIFunction"));

				sendSpy.clear();
			}
		}


};


QTEST_GUILESS_MAIN(test_ServerMessageHandler)
#include "test_ServerMessageHandler.moc"
