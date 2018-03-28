/*!
 * \brief Unit tests for \ref RemoteReaderManagerPlugIn
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteReaderManagerPlugIn.h"

#include "messages/IfdConnect.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdError.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdStatus.h"
#include "messages/IfdTransmit.h"
#include "messages/RemoteMessageParser.h"
#include "MockDataChannel.h"
#include "MockIfdStatus.h"
#include "MockRemoteDispatcher.h"
#include "RemoteClient.h"

#include <QQueue>
#include <QSemaphore>
#include <QtCore/QtCore>
#include <QtTest/QtTest>


using namespace governikus;


Q_DECLARE_METATYPE(QSharedPointer<const RemoteMessage> )


class MockRemoteClient
	: public RemoteClient
{
	Q_OBJECT

	public:
		MockRemoteClient() = default;
		virtual ~MockRemoteClient() override = default;

		virtual void startDetection() override;
		virtual void stopDetection() override;
		virtual bool isDetecting() override;
		virtual void establishConnection(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const QString& pPsk) override;
		virtual QVector<RemoteServiceSettings::RemoteInfo> getConnectedDeviceInfos() override;

};


void MockRemoteClient::startDetection()
{
}


void MockRemoteClient::stopDetection()
{
}


bool MockRemoteClient::isDetecting()
{
	return false;
}


void MockRemoteClient::establishConnection(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const QString& pPsk)
{
	Q_UNUSED(pEntry);
	Q_UNUSED(pPsk);
}


QVector<RemoteServiceSettings::RemoteInfo> MockRemoteClient::getConnectedDeviceInfos()
{
	return QVector<RemoteServiceSettings::RemoteInfo>();
}


class test_RemoteReaderManagerPlugIn
	: public QObject
{
	Q_OBJECT

	private:
		QThread mNetworkThread;
		QSharedPointer<RemoteReaderManagerPlugIn> mPlugin;
		QSharedPointer<MockRemoteClient> mRemoteClient;
		QSharedPointer<MockRemoteDispatcher> mDispatcher1;
		QSharedPointer<MockRemoteDispatcher> mDispatcher2;
		QVector<QSharedPointer<const RemoteMessage> > mClientMessages;

		void waitForSignals(int identifier, QSignalSpy* const pSpy, const int pExpectedCount, const int pTimeoutMs)
		{
			qDebug() << "waitForSignals:" << identifier;
			for (int tryCount = 0; pSpy->count() < pExpectedCount && tryCount < pExpectedCount; ++tryCount)
			{
				pSpy->wait(pTimeoutMs);
			}
			QCOMPARE(pSpy->count(), pExpectedCount);
		}


	private Q_SLOTS:
		void initTestCase()
		{
			const RemoteMessageParser parser;

			mNetworkThread.setObjectName(QStringLiteral("NetworkThread"));

			mRemoteClient.reset(new MockRemoteClient);

			mDispatcher1.reset(new MockRemoteDispatcher());
			mDispatcher1->moveToThread(&mNetworkThread);

			mDispatcher2.reset(new MockRemoteDispatcher());
			mDispatcher2->moveToThread(&mNetworkThread);

			mNetworkThread.start();

			auto m = [&](const char* pJsonByteData) -> QSharedPointer<const RemoteMessage>
					{
						const QByteArray jsonData(pJsonByteData);
						return parser.parse(jsonData);
					};

			mClientMessages
				<< m("{\n"
					 "    \"msg\": \"IFDEstablishContext\",\n"
					 "    \"Protocol\": \"IFDInterface_WebSocket_v0\",\n"
					 "	\"UDName\": \"MAC-MINI\"\n"
					 "}")
				<< m("{\n"
				 "    \"ContextHandle\": \"contextHandle\",\n"
				 "    \"msg\": \"IFDGetStatus\",\n"
				 "    \"SlotName\": \"Remote Reader\"\n"
				 "}")
				<< m("{\n"
				 "    \"ContextHandle\": \"contextHandle\",\n"
				 "    \"exclusive\": true,\n"
				 "    \"msg\": \"IFDConnect\",\n"
				 "    \"SlotName\": \"NFC Reader\"\n"
				 "}")
				<< m("{\n"
				 "    \"ContextHandle\": \"contextHandle\",\n"
				 "    \"SlotHandle\": \"NFC Reader\",\n"
				 "    \"msg\": \"IFDDisconnect\",\n"
				 "    \"readerName\": \"NFC Reader\"\n"
				 "}")
				<< m("{\n"
				 "    \"ContextHandle\": \"contextHandle\",\n"
				 "    \"msg\": \"IFDTransmit\",\n"
				 "    \"SlotHandle\": \"NFC Reader\",\n"
				 "    \"CommandAPDUs\": [\n"
				 "        {\n"
				 "            \"InputAPDU\": \"00A402022F00\",\n"
				 "            \"AcceptableStatusCodes\": null\n"
				 "        }\n"
				 "    ]\n"
				 "}")
				<< m("{\n"
				 "    \"ContextHandle\": \"TestContext\",\n"
				 "    \"InputData\": \"abcd1234\",\n"
				 "    \"SlotHandle\": \"My little Reader\",\n"
				 "    \"msg\": \"IFDEstablishPACEChannel\"\n"
				 "}\n");
		}


		void cleanupTestCase()
		{
			mNetworkThread.quit();
			mNetworkThread.wait();
		}


		void init()
		{
			// local
			mPlugin.reset(new RemoteReaderManagerPlugIn());
			mPlugin->setRemoteClient(mRemoteClient);
		}


		void cleanup()
		{
			// local
		}


		void testStandardValues()
		{
			QSignalSpy spySend(mDispatcher1.data(), &MockRemoteDispatcher::fireSend);

			QVERIFY(mPlugin->getReaders().isEmpty());

			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher1);
			waitForSignals(1, &spySend, 1, 1000);

			QVERIFY(mPlugin->getReaders().isEmpty());
			QCOMPARE(spySend.size(), 1);

			QSharedPointer<const RemoteMessage> result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDEstablishContext);
		}


		void testSingleDispatcherSingleReaderAddRemoveWithoutCard()
		{
			QSignalSpy spySend(mDispatcher1.data(), &MockRemoteDispatcher::fireSend);

			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher1);
			waitForSignals(2, &spySend, 1, 1000);
			spySend.clear();

			QSharedPointer<RemoteMessage> message;
			QSignalSpy spyAdded(mPlugin.data(), &ReaderManagerPlugIn::fireReaderAdded);
			QSignalSpy spyRemoved(mPlugin.data(), &ReaderManagerPlugIn::fireReaderRemoved);

			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader"), PaceCapabilities(), 500, true));
			mDispatcher1->onReceived(message);
			QCOMPARE(mPlugin->getReaders().size(), 1);
			QCOMPARE(spySend.size(), 0);
			QCOMPARE(spyAdded.size(), 1);
			QCOMPARE(spyAdded.takeFirst()[0].toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(mPlugin->getReaders().size(), 1);
			QCOMPARE(mPlugin->getReaders()[0]->getName(), QStringLiteral("NFC Reader"));
			QCOMPARE(mPlugin->getReaders()[0]->getReaderInfo().isBasicReader(), true);
			QCOMPARE(mPlugin->getReaders()[0]->getReaderInfo().getMaxApduLength(), 500);

			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader"), PaceCapabilities(), 500, false));
			mDispatcher1->onReceived(message);
			QCOMPARE(mPlugin->getReaders().size(), 0);
			QCOMPARE(spySend.size(), 0);
			QCOMPARE(spyAdded.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(spyRemoved.takeFirst()[0].toString(), QStringLiteral("NFC Reader"));

			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader"), PaceCapabilities(), 500, true));
			mDispatcher1->onReceived(message);
			QCOMPARE(mPlugin->getReaders().size(), 1);
			QCOMPARE(mPlugin->getReaders()[0]->getName(), QStringLiteral("NFC Reader"));
			spyAdded.clear();

			mDispatcher1->onClosed();
			QCOMPARE(spySend.size(), 0);
			QCOMPARE(spyAdded.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(spyRemoved.takeFirst()[0].toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(mPlugin->getReaders().size(), 0);
		}


		void testSingleDispatcherSingleReaderPaceCapabilities()
		{
			QSignalSpy spySend(mDispatcher1.data(), &MockRemoteDispatcher::fireSend);

			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher1);
			waitForSignals(3, &spySend, 1, 1000);
			spySend.clear();

			QSharedPointer<RemoteMessage> message;
			QSignalSpy spyAdded(mPlugin.data(), &ReaderManagerPlugIn::fireReaderAdded);
			QSignalSpy spyRemoved(mPlugin.data(), &ReaderManagerPlugIn::fireReaderRemoved);

			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader"), PaceCapabilities(true), 500, true));
			mDispatcher1->onReceived(message);
			QCOMPARE(spySend.size(), 0);
			QCOMPARE(spyAdded.size(), 1);
			QCOMPARE(spyAdded.takeFirst()[0].toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(mPlugin->getReaders().size(), 1);
			QCOMPARE(mPlugin->getReaders()[0]->getName(), QStringLiteral("NFC Reader"));
			QCOMPARE(mPlugin->getReaders()[0]->getReaderInfo().isBasicReader(), false);
			QCOMPARE(mPlugin->getReaders()[0]->getReaderInfo().getMaxApduLength(), 500);
		}


		void testMultipleDispatcherSingleReaderAddRemoveWithoutCard()
		{
			QSignalSpy spySend1(mDispatcher1.data(), &MockRemoteDispatcher::fireSend);
			QSignalSpy spySend2(mDispatcher2.data(), &MockRemoteDispatcher::fireSend);

			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher1);
			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher2);
			waitForSignals(4, &spySend1, 1, 1000);
			waitForSignals(5, &spySend2, 1, 1000);
			spySend1.clear();
			spySend2.clear();

			QSharedPointer<RemoteMessage> message;
			QSignalSpy spyAdded(mPlugin.data(), &ReaderManagerPlugIn::fireReaderAdded);
			QSignalSpy spyRemoved(mPlugin.data(), &ReaderManagerPlugIn::fireReaderRemoved);

			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader 1"), PaceCapabilities(), 500, true));
			mDispatcher1->onReceived(message);
			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader 2"), PaceCapabilities(), 500, true));
			mDispatcher2->onReceived(message);
			QCOMPARE(spySend1.size(), 0);
			QCOMPARE(spySend2.size(), 0);
			QCOMPARE(spyAdded.size(), 2);
			QCOMPARE(spyAdded.takeFirst()[0].toString(), QStringLiteral("NFC Reader 1"));
			QCOMPARE(spyAdded.takeFirst()[0].toString(), QStringLiteral("NFC Reader 2"));
			QCOMPARE(spyRemoved.size(), 0);
			QCOMPARE(mPlugin->getReaders().size(), 2);
			QCOMPARE(mPlugin->getReaders()[0]->getName(), QStringLiteral("NFC Reader 1"));
			QCOMPARE(mPlugin->getReaders()[1]->getName(), QStringLiteral("NFC Reader 2"));

			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader 1"), PaceCapabilities(), 500, false));
			mDispatcher1->onReceived(message);
			QCOMPARE(spySend1.size(), 0);
			QCOMPARE(spySend2.size(), 0);
			QCOMPARE(spyAdded.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(spyRemoved.takeFirst()[0].toString(), QStringLiteral("NFC Reader 1"));
			QCOMPARE(mPlugin->getReaders().size(), 1);
			QCOMPARE(mPlugin->getReaders()[0]->getName(), QStringLiteral("NFC Reader 2"));

			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader 1"), PaceCapabilities(), 500, true));
			mDispatcher1->onReceived(message);
			QCOMPARE(spySend1.size(), 0);
			QCOMPARE(spySend2.size(), 0);
			QCOMPARE(spyAdded.size(), 1);
			QCOMPARE(spyAdded.takeFirst()[0].toString(), QStringLiteral("NFC Reader 1"));
			QCOMPARE(spyRemoved.size(), 0);
			QCOMPARE(mPlugin->getReaders().size(), 2);
			QCOMPARE(mPlugin->getReaders()[0]->getName(), QStringLiteral("NFC Reader 1"));
			QCOMPARE(mPlugin->getReaders()[1]->getName(), QStringLiteral("NFC Reader 2"));

			mDispatcher1->onClosed();
			QCOMPARE(spySend1.size(), 0);
			QCOMPARE(spySend2.size(), 0);
			QCOMPARE(spyAdded.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(spyRemoved.takeFirst()[0].toString(), QStringLiteral("NFC Reader 1"));
			QCOMPARE(mPlugin->getReaders().size(), 1);
			QCOMPARE(mPlugin->getReaders()[0]->getName(), QStringLiteral("NFC Reader 2"));

			mDispatcher2->onClosed();
			QCOMPARE(spySend1.size(), 0);
			QCOMPARE(spySend2.size(), 0);
			QCOMPARE(spyAdded.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(spyRemoved.takeFirst()[0].toString(), QStringLiteral("NFC Reader 2"));
			QCOMPARE(mPlugin->getReaders().size(), 0);
		}


		void testSingleDispatcherSingleReaderChangeCard()
		{
			QSharedPointer<const RemoteMessage> result;
			QSignalSpy spySend(mDispatcher1.data(), &MockRemoteDispatcher::fireSend);

			mDispatcher1->setState(MockRemoteDispatcher::DispatcherState::ReaderWithCardError);
			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher1);

			waitForSignals(6, &spySend, 2, 1000);
			spySend.takeFirst();
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDConnect);

			QSharedPointer<RemoteMessage> message;
			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader"), PaceCapabilities(), 500, true, false));
			mDispatcher1->onReceived(message);
			QCOMPARE(mPlugin->getReaders()[0]->getCard(), nullptr);

			QSignalSpy spyInserted(mPlugin->getReaders()[0], &Reader::fireCardInserted);
			QSignalSpy spyRemoved(mPlugin->getReaders()[0], &Reader::fireCardRemoved);
			QSignalSpy spyChanged(mPlugin->getReaders()[0], &Reader::fireCardRetryCounterChanged);
			QSignalSpy spyUpdated(mPlugin->getReaders()[0], &Reader::fireReaderPropertiesUpdated);

			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader"), PaceCapabilities(), 500, true, true));
			mDispatcher1->onReceived(message);

			waitForSignals(7, &spySend, 1, 1000);
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDConnect);
			QVERIFY(mPlugin->getReaders()[0]->getCard() != nullptr);
			QCOMPARE(mPlugin->getReaders()[0]->getReaderInfo().getMaxApduLength(), 500);
			QVERIFY(mPlugin->getReaders()[0]->getReaderInfo().hasCard());
			QCOMPARE(spyInserted.size(), 1);
			QCOMPARE(spyInserted.takeFirst()[0].toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(spyRemoved.size(), 0);
			QCOMPARE(spyChanged.size(), 0);
			QCOMPARE(spyUpdated.size(), 0);

			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader"), PaceCapabilities(), 1, true, true));
			mDispatcher1->onReceived(message);
			QVERIFY(mPlugin->getReaders()[0]->getCard() != nullptr);
			QCOMPARE(mPlugin->getReaders()[0]->getReaderInfo().getMaxApduLength(), 1);
			QVERIFY(mPlugin->getReaders()[0]->getReaderInfo().hasCard());
			QCOMPARE(spyInserted.size(), 0);
			QCOMPARE(spyRemoved.size(), 0);
			QCOMPARE(spyChanged.size(), 0);
			QCOMPARE(spyUpdated.size(), 1);
			QCOMPARE(spyUpdated.takeFirst()[0].toString(), QStringLiteral("NFC Reader"));

			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader"), PaceCapabilities(), 1, true, true));
			mDispatcher1->onReceived(message);
			QVERIFY(mPlugin->getReaders()[0]->getCard() != nullptr);
			QCOMPARE(mPlugin->getReaders()[0]->getReaderInfo().getMaxApduLength(), 1);
			QVERIFY(mPlugin->getReaders()[0]->getReaderInfo().hasCard());
			QCOMPARE(spyInserted.size(), 0);
			QCOMPARE(spyRemoved.size(), 0);
			QCOMPARE(spyChanged.size(), 0);
			QCOMPARE(spyUpdated.size(), 0);

			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader"), PaceCapabilities(), 1, true, false));
			mDispatcher1->onReceived(message);
			QCOMPARE(mPlugin->getReaders()[0]->getCard(), nullptr);
			QVERIFY(!mPlugin->getReaders()[0]->getReaderInfo().hasCard());
			QCOMPARE(spyInserted.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(spyRemoved.takeFirst()[0].toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(spyChanged.size(), 0);
			QCOMPARE(spyUpdated.size(), 0);

			message.reset(new MockIfdStatus(QStringLiteral("NFC Reader"), PaceCapabilities(), 1, true, true));
			mDispatcher1->onReceived(message);
			waitForSignals(8, &spySend, 1, 1000);
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDConnect);
			QVERIFY(mPlugin->getReaders()[0]->getCard() != nullptr);
			QCOMPARE(mPlugin->getReaders()[0]->getReaderInfo().getMaxApduLength(), 1);
			QVERIFY(mPlugin->getReaders()[0]->getReaderInfo().hasCard());
			QCOMPARE(spyInserted.size(), 1);
			QCOMPARE(spyInserted.takeFirst()[0].toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(spyRemoved.size(), 0);
			QCOMPARE(spyChanged.size(), 0);
			QCOMPARE(spyUpdated.size(), 0);
		}


		void testSingleDispatcherSingleReaderCardCommunication()
		{
			QSharedPointer<const RemoteMessage> result;
			QSignalSpy spySend(mDispatcher1.data(), &MockRemoteDispatcher::fireSend);

			mDispatcher1->setState(MockRemoteDispatcher::DispatcherState::ReaderWithCard);
			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher1);

			waitForSignals(9, &spySend, 4, 1000);
			spySend.takeFirst();
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDConnect);
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDTransmit);
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDDisconnect);

			QCOMPARE(mPlugin->getReaders().size(), 1);
			Card* card = mPlugin->getReaders().at(0)->getCard();
			QVERIFY(card != nullptr);

			QCOMPARE(card->connect(), CardReturnCode::OK);
			waitForSignals(10, &spySend, 1, 1000);
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDConnect);
			QCOMPARE(static_cast<const IfdConnect*>(result.data())->getSlotName(), QStringLiteral("NFC Reader"));

			QCOMPARE(card->disconnect(), CardReturnCode::OK);
			waitForSignals(11, &spySend, 1, 1000);
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDDisconnect);
			QCOMPARE(static_cast<const IfdDisconnect*>(result.data())->getSlotHandle(), QStringLiteral("NFC Reader"));

			CommandApdu cmd(QByteArray("ping"));
			ResponseApdu res;
			QCOMPARE(card->transmit(cmd, res), CardReturnCode::OK);
			waitForSignals(12, &spySend, 1, 1000);
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(static_cast<const IfdTransmit*>(result.data())->getSlotHandle(), QStringLiteral("NFC Reader"));
			QCOMPARE(static_cast<const IfdTransmit*>(result.data())->getInputApdu(), QByteArray("ping"));
			QCOMPARE(res.getBuffer(), QByteArray("pong"));

			mDispatcher1->setState(MockRemoteDispatcher::DispatcherState::ReaderWithCardError);

			QCOMPARE(card->connect(), CardReturnCode::COMMAND_FAILED);
			waitForSignals(13, &spySend, 1, 1000);
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDConnect);
			QCOMPARE(static_cast<const IfdConnect*>(result.data())->getSlotName(), QStringLiteral("NFC Reader"));

			QCOMPARE(card->disconnect(), CardReturnCode::COMMAND_FAILED);
			waitForSignals(14, &spySend, 1, 1000);
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDDisconnect);
			QCOMPARE(static_cast<const IfdDisconnect*>(result.data())->getSlotHandle(), QStringLiteral("NFC Reader"));

			res.setBuffer(QByteArray());
			QCOMPARE(card->transmit(cmd, res), CardReturnCode::COMMAND_FAILED);
			waitForSignals(15, &spySend, 1, 1000);
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(static_cast<const IfdTransmit*>(result.data())->getSlotHandle(), QStringLiteral("NFC Reader"));
			QCOMPARE(static_cast<const IfdTransmit*>(result.data())->getInputApdu(), QByteArray("ping"));
			QCOMPARE(res.getBuffer(), QByteArray());
		}


		void testUnexpectedMessagesCauseAnIfdErrorMessage()
		{
			QSignalSpy spySend(mDispatcher1.data(), &MockRemoteDispatcher::fireSend);

			mDispatcher1->setState(MockRemoteDispatcher::DispatcherState::WithoutReader);
			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher1);
			waitForSignals(1, &spySend, 1, 1000);
			spySend.clear();

			for (const QSharedPointer<const RemoteMessage>& clientMessage : qAsConst(mClientMessages))
			{
				QMetaObject::invokeMethod(mDispatcher1.data(), "onReceived", Qt::QueuedConnection, Q_ARG(QSharedPointer<const RemoteMessage>, clientMessage));
				waitForSignals(2, &spySend, 1, 1000);
				const QList<QVariant>& arguments = spySend.last();

				const QVariant remoteMessageVariant = arguments.at(0);
				QVERIFY(remoteMessageVariant.canConvert<QSharedPointer<const RemoteMessage> >());
				const QSharedPointer<const RemoteMessage> message = remoteMessageVariant.value<QSharedPointer<const RemoteMessage> >();
				const QSharedPointer<const IfdError> errorMessage = message.dynamicCast<const IfdError>();

				QVERIFY(!errorMessage.isNull());
				QCOMPARE(errorMessage->getType(), RemoteCardMessageType::IFDError);
				QCOMPARE(errorMessage->getContextHandle(), QString());
				QCOMPARE(errorMessage->getSlotHandle(), QString());
				QVERIFY(errorMessage->resultHasError());
				QCOMPARE(errorMessage->getResultMinor(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#unknownAPIFunction"));

				spySend.clear();
			}
		}


};

QTEST_GUILESS_MAIN(test_RemoteReaderManagerPlugIn)
#include "test_RemoteReaderManagerPlugin.moc"
