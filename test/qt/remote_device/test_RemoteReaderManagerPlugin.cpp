/*!
 * \brief Unit tests for \ref RemoteReaderManagerPlugIn
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "plugin/RemoteReaderManagerPlugIn.h"

#include "AppSettings.h"
#include "Env.h"
#include "messages/IfdConnect.h"
#include "messages/IfdConnect.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdError.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdGetStatus.h"
#include "messages/IfdStatus.h"
#include "messages/IfdTransmit.h"
#include "messages/IfdTransmit.h"
#include "MockRemoteDispatcher.h"
#include "RemoteClient.h"

#include <QQueue>
#include <QSemaphore>
#include <QtCore>
#include <QtTest>

using namespace governikus;


Q_DECLARE_METATYPE(QSharedPointer<const RemoteMessage>)
Q_DECLARE_METATYPE(ReaderInfo)


class MockRemoteClient
	: public RemoteClient
{
	Q_OBJECT

	public:
		MockRemoteClient() = default;
		~MockRemoteClient() override = default;

		void startDetection() override;
		void stopDetection() override;
		bool isDetecting() override;
		void establishConnection(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const QString& pPsk) override;
		QVector<RemoteServiceSettings::RemoteInfo> getConnectedDeviceInfos() override;
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
	Q_UNUSED(pEntry)
	Q_UNUSED(pPsk)
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
		QSharedPointer<MockRemoteClient> mRemoteClient;
		QSharedPointer<RemoteReaderManagerPlugIn> mPlugin;
		QSharedPointer<MockRemoteDispatcher> mDispatcher1;
		QSharedPointer<MockRemoteDispatcher> mDispatcher2;
		QVector<QSharedPointer<const RemoteMessage> > mClientMessages;

		ReaderInfo getReaderInfo(QSignalSpy& pSpy)
		{
			return qvariant_cast<ReaderInfo>(pSpy.takeFirst().at(0));
		}

	private Q_SLOTS:
		void init()
		{
			mNetworkThread.setObjectName(QStringLiteral("NetworkThread"));
			mNetworkThread.start();

			mRemoteClient.reset(new MockRemoteClient());
			Env::set(RemoteClient::staticMetaObject, mRemoteClient.data());

			mDispatcher1.reset(new MockRemoteDispatcher());
			mDispatcher1->moveToThread(&mNetworkThread);

			mDispatcher2.reset(new MockRemoteDispatcher());
			mDispatcher2->moveToThread(&mNetworkThread);

			mPlugin.reset(new RemoteReaderManagerPlugIn());
			mPlugin->init();
		}


		void cleanup()
		{
			mRemoteClient.reset();
			mDispatcher1.reset();
			mDispatcher2.reset();
			mPlugin.reset();

			mNetworkThread.quit();
			mNetworkThread.wait();
		}


		void testStandardValues()
		{
			QSignalSpy spySend(mDispatcher1.data(), &MockRemoteDispatcher::fireSend);

			QVERIFY(mPlugin->getReaders().isEmpty());

			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher1);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations

			QVERIFY(mPlugin->getReaders().isEmpty());
			QCOMPARE(spySend.size(), 1);

			QSharedPointer<const RemoteMessage> result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDEstablishContext);
		}


		void testSingleDispatcherSingleReaderAddRemoveWithoutCard()
		{
			QSignalSpy spySend(mDispatcher1.data(), &MockRemoteDispatcher::fireSend);

			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher1);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			spySend.clear();

			QSharedPointer<RemoteMessage> message;
			QSignalSpy spyAdded(mPlugin.data(), &ReaderManagerPlugIn::fireReaderAdded);
			QSignalSpy spyRemoved(mPlugin.data(), &ReaderManagerPlugIn::fireReaderRemoved);

			ReaderInfo info(QStringLiteral("NFC Reader"));
			info.setConnected(true);
			info.setBasicReader(true);
			info.setMaxApduLength(500);
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(false);
			message.reset(new IfdStatus(info));
			mDispatcher1->onReceived(message);
			QCOMPARE(mPlugin->getReaders().size(), 1);
			QCOMPARE(spySend.size(), 0);
			QCOMPARE(spyAdded.size(), 1);
			QCOMPARE(getReaderInfo(spyAdded).getName(), QStringLiteral("NFC Reader#TestContext"));
			QCOMPARE(mPlugin->getReaders().size(), 1);
			QCOMPARE(mPlugin->getReaders().at(0)->getName(), QStringLiteral("NFC Reader#TestContext"));
			QCOMPARE(mPlugin->getReaders().at(0)->getReaderInfo().isBasicReader(), true);
			QCOMPARE(mPlugin->getReaders().at(0)->getReaderInfo().getMaxApduLength(), 500);

			info.setConnected(false);
			message.reset(new IfdStatus(info));
			mDispatcher1->onReceived(message);
			QCOMPARE(mPlugin->getReaders().size(), 0);
			QCOMPARE(spySend.size(), 0);
			QCOMPARE(spyAdded.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(getReaderInfo(spyRemoved).getName(), QStringLiteral("NFC Reader#TestContext"));

			info.setConnected(true);
			message.reset(new IfdStatus(info));
			mDispatcher1->onReceived(message);
			QCOMPARE(mPlugin->getReaders().size(), 1);
			QCOMPARE(mPlugin->getReaders().at(0)->getName(), QStringLiteral("NFC Reader#TestContext"));
			spyAdded.clear();

			mDispatcher1->onClosed();
			QCOMPARE(spySend.size(), 0);
			QCOMPARE(spyAdded.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(getReaderInfo(spyRemoved).getName(), QStringLiteral("NFC Reader#TestContext"));
			QCOMPARE(mPlugin->getReaders().size(), 0);
		}


		void testSingleDispatcherSingleReaderPaceCapabilities()
		{
			QSignalSpy spySend(mDispatcher1.data(), &MockRemoteDispatcher::fireSend);

			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher1);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			spySend.clear();

			QSharedPointer<RemoteMessage> message;
			QSignalSpy spyAdded(mPlugin.data(), &ReaderManagerPlugIn::fireReaderAdded);
			QSignalSpy spyRemoved(mPlugin.data(), &ReaderManagerPlugIn::fireReaderRemoved);

			ReaderInfo info(QStringLiteral("NFC Reader"));
			info.setConnected(true);
			info.setBasicReader(true);
			info.setMaxApduLength(500);
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(true);
			message.reset(new IfdStatus(info));
			mDispatcher1->onReceived(message);
			QCOMPARE(spySend.size(), 0);
			QCOMPARE(spyAdded.size(), 1);
			const auto spyInfo = getReaderInfo(spyAdded);
			QCOMPARE(spyInfo.getName(), QStringLiteral("NFC Reader#TestContext"));
			QCOMPARE(spyInfo.getMaxApduLength(), 500);
			QCOMPARE(mPlugin->getReaders().size(), 1);
			QCOMPARE(mPlugin->getReaders().at(0)->getName(), QStringLiteral("NFC Reader#TestContext"));
			QCOMPARE(mPlugin->getReaders().at(0)->getReaderInfo().isBasicReader(), false);
			QCOMPARE(mPlugin->getReaders().at(0)->getReaderInfo().getMaxApduLength(), 500);
		}


		void testMultipleDispatcherSingleReaderAddRemoveWithoutCard()
		{
			QSignalSpy spySend1(mDispatcher1.data(), &MockRemoteDispatcher::fireSend);
			QSignalSpy spySend2(mDispatcher2.data(), &MockRemoteDispatcher::fireSend);

			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher1);
			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher2);
			QTRY_COMPARE(spySend1.count(), 1); // clazy:exclude=qstring-allocations
			QTRY_COMPARE(spySend2.count(), 1); // clazy:exclude=qstring-allocations
			spySend1.clear();
			spySend2.clear();

			QSharedPointer<RemoteMessage> message;
			QSignalSpy spyAdded(mPlugin.data(), &ReaderManagerPlugIn::fireReaderAdded);
			QSignalSpy spyRemoved(mPlugin.data(), &ReaderManagerPlugIn::fireReaderRemoved);

			ReaderInfo info1(QStringLiteral("NFC Reader 1"));
			info1.setMaxApduLength(500);
			info1.setConnected(true);
			info1.setBasicReader(true);
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(false);
			message.reset(new IfdStatus(info1));
			mDispatcher1->onReceived(message);

			ReaderInfo info2(QStringLiteral("NFC Reader 2"));
			info2.setMaxApduLength(500);
			info2.setConnected(true);
			info2.setBasicReader(true);
			message.reset(new IfdStatus(info2));
			mDispatcher2->onReceived(message);

			QCOMPARE(spySend1.size(), 0);
			QCOMPARE(spySend2.size(), 0);
			QCOMPARE(spyAdded.size(), 2);
			QCOMPARE(getReaderInfo(spyAdded).getName(), QStringLiteral("NFC Reader 1#TestContext"));
			QCOMPARE(getReaderInfo(spyAdded).getName(), QStringLiteral("NFC Reader 2#TestContext"));
			QCOMPARE(spyRemoved.size(), 0);
			QCOMPARE(mPlugin->getReaders().size(), 2);
			QCOMPARE(mPlugin->getReaders().at(0)->getName(), QStringLiteral("NFC Reader 1#TestContext"));
			QCOMPARE(mPlugin->getReaders().at(1)->getName(), QStringLiteral("NFC Reader 2#TestContext"));

			info1.setConnected(false);
			message.reset(new IfdStatus(info1));
			mDispatcher1->onReceived(message);

			QCOMPARE(spySend1.size(), 0);
			QCOMPARE(spySend2.size(), 0);
			QCOMPARE(spyAdded.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(getReaderInfo(spyRemoved).getName(), QStringLiteral("NFC Reader 1#TestContext"));
			QCOMPARE(mPlugin->getReaders().size(), 1);
			QCOMPARE(mPlugin->getReaders().at(0)->getName(), QStringLiteral("NFC Reader 2#TestContext"));

			info1.setConnected(true);
			message.reset(new IfdStatus(info1));
			mDispatcher1->onReceived(message);

			QCOMPARE(spySend1.size(), 0);
			QCOMPARE(spySend2.size(), 0);
			QCOMPARE(spyAdded.size(), 1);
			QCOMPARE(getReaderInfo(spyAdded).getName(), QStringLiteral("NFC Reader 1#TestContext"));
			QCOMPARE(spyRemoved.size(), 0);
			QCOMPARE(mPlugin->getReaders().size(), 2);
			QCOMPARE(mPlugin->getReaders().at(0)->getName(), QStringLiteral("NFC Reader 1#TestContext"));
			QCOMPARE(mPlugin->getReaders().at(1)->getName(), QStringLiteral("NFC Reader 2#TestContext"));

			mDispatcher1->onClosed();
			QCOMPARE(spySend1.size(), 0);
			QCOMPARE(spySend2.size(), 0);
			QCOMPARE(spyAdded.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(getReaderInfo(spyRemoved).getName(), QStringLiteral("NFC Reader 1#TestContext"));
			QCOMPARE(mPlugin->getReaders().size(), 1);
			QCOMPARE(mPlugin->getReaders().at(0)->getName(), QStringLiteral("NFC Reader 2#TestContext"));

			mDispatcher2->onClosed();
			QCOMPARE(spySend1.size(), 0);
			QCOMPARE(spySend2.size(), 0);
			QCOMPARE(spyAdded.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(getReaderInfo(spyRemoved).getName(), QStringLiteral("NFC Reader 2#TestContext"));
			QCOMPARE(mPlugin->getReaders().size(), 0);
		}


		void testSingleDispatcherSingleReaderChangeCard()
		{
			QSharedPointer<const RemoteMessage> result;
			QSignalSpy spySend(mDispatcher1.data(), &MockRemoteDispatcher::fireSend);

			mDispatcher1->setState(MockRemoteDispatcher::DispatcherState::ReaderWithCardError);
			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher1);

			QTRY_COMPARE(spySend.count(), 2); // clazy:exclude=qstring-allocations
			spySend.takeFirst();
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDConnect);

			QSharedPointer<RemoteMessage> message;

			ReaderInfo info1(QStringLiteral("NFC Reader"));
			info1.setMaxApduLength(500);
			info1.setConnected(true);
			message.reset(new IfdStatus(info1));
			info1.setBasicReader(true);
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(false);
			mDispatcher1->onReceived(message);
			QCOMPARE(mPlugin->getReaders().at(0)->getCard(), nullptr);

			QSignalSpy spyInserted(mPlugin->getReaders().at(0), &Reader::fireCardInserted);
			QSignalSpy spyRemoved(mPlugin->getReaders().at(0), &Reader::fireCardRemoved);
			QSignalSpy spyChanged(mPlugin->getReaders().at(0), &Reader::fireCardRetryCounterChanged);
			QSignalSpy spyUpdated(mPlugin->getReaders().at(0), &Reader::fireReaderPropertiesUpdated);

			info1.setCardInfo(CardInfo(CardType::EID_CARD));
			message.reset(new IfdStatus(info1));
			mDispatcher1->onReceived(message);

			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDConnect);
			QVERIFY(mPlugin->getReaders().at(0)->getCard() != nullptr);
			QCOMPARE(mPlugin->getReaders().at(0)->getReaderInfo().getMaxApduLength(), 500);
			QVERIFY(mPlugin->getReaders().at(0)->getReaderInfo().hasCard());
			QCOMPARE(spyInserted.size(), 1);
			QCOMPARE(getReaderInfo(spyInserted).getName(), QStringLiteral("NFC Reader#TestContext"));
			QCOMPARE(spyRemoved.size(), 0);
			QCOMPARE(spyChanged.size(), 0);
			QCOMPARE(spyUpdated.size(), 0);

			info1.setMaxApduLength(1);
			message.reset(new IfdStatus(info1));
			mDispatcher1->onReceived(message);
			QVERIFY(mPlugin->getReaders().at(0)->getCard() != nullptr);
			QCOMPARE(mPlugin->getReaders().at(0)->getReaderInfo().getMaxApduLength(), 1);
			QVERIFY(mPlugin->getReaders().at(0)->getReaderInfo().hasCard());
			QCOMPARE(spyInserted.size(), 0);
			QCOMPARE(spyRemoved.size(), 0);
			QCOMPARE(spyChanged.size(), 0);
			QCOMPARE(spyUpdated.size(), 1);
			QCOMPARE(getReaderInfo(spyUpdated).getName(), QStringLiteral("NFC Reader#TestContext"));

			message.reset(new IfdStatus(info1));
			mDispatcher1->onReceived(message);
			QVERIFY(mPlugin->getReaders().at(0)->getCard() != nullptr);
			QCOMPARE(mPlugin->getReaders().at(0)->getReaderInfo().getMaxApduLength(), 1);
			QVERIFY(mPlugin->getReaders().at(0)->getReaderInfo().hasCard());
			QCOMPARE(spyInserted.size(), 0);
			QCOMPARE(spyRemoved.size(), 0);
			QCOMPARE(spyChanged.size(), 0);
			QCOMPARE(spyUpdated.size(), 0);

			ReaderInfo info2(QStringLiteral("NFC Reader"));
			info2.setMaxApduLength(1);
			info2.setConnected(true);
			info2.setBasicReader(true);
			message.reset(new IfdStatus(info2));
			mDispatcher1->onReceived(message);
			QCOMPARE(mPlugin->getReaders().at(0)->getCard(), nullptr);
			QVERIFY(!mPlugin->getReaders().at(0)->getReaderInfo().hasCard());
			QCOMPARE(spyInserted.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(getReaderInfo(spyRemoved).getName(), QStringLiteral("NFC Reader#TestContext"));
			QCOMPARE(spyChanged.size(), 0);
			QCOMPARE(spyUpdated.size(), 0);

			info1.setCardInfo(CardInfo(CardType::EID_CARD));
			message.reset(new IfdStatus(info1));
			mDispatcher1->onReceived(message);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDConnect);
			QVERIFY(mPlugin->getReaders().at(0)->getCard() != nullptr);
			QCOMPARE(mPlugin->getReaders().at(0)->getReaderInfo().getMaxApduLength(), 1);
			QVERIFY(mPlugin->getReaders().at(0)->getReaderInfo().hasCard());
			QCOMPARE(spyInserted.size(), 1);
			QCOMPARE(getReaderInfo(spyInserted).getName(), QStringLiteral("NFC Reader#TestContext"));
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

			QTRY_COMPARE(spySend.count(), 8); // clazy:exclude=qstring-allocations
			spySend.takeFirst();
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDConnect);
			for (int i = 0; i < 5; ++i) // CardInfo detection
			{
				result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
				QCOMPARE(result->getType(), RemoteCardMessageType::IFDTransmit);
			}
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDDisconnect);

			QCOMPARE(mPlugin->getReaders().size(), 1);
			Card* card = mPlugin->getReaders().at(0)->getCard();
			QVERIFY(card != nullptr);

			QCOMPARE(card->connect(), CardReturnCode::OK);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDConnect);
			QCOMPARE(static_cast<const IfdConnect*>(result.data())->getSlotName(), QStringLiteral("NFC Reader"));

			QCOMPARE(card->disconnect(), CardReturnCode::OK);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDDisconnect);
			QCOMPARE(static_cast<const IfdDisconnect*>(result.data())->getSlotHandle(), QStringLiteral("NFC Reader"));

			CommandApdu cmd(QByteArray("ping"));
			ResponseApduResult res = card->transmit(cmd);
			QCOMPARE(res.mReturnCode, CardReturnCode::OK);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(static_cast<const IfdTransmit*>(result.data())->getSlotHandle(), QStringLiteral("NFC Reader"));
			QCOMPARE(static_cast<const IfdTransmit*>(result.data())->getInputApdu(), QByteArray("ping"));
			QCOMPARE(res.mResponseApdu.getBuffer(), QByteArray("pong"));

			mDispatcher1->setState(MockRemoteDispatcher::DispatcherState::ReaderWithCardError);

			QCOMPARE(card->connect(), CardReturnCode::COMMAND_FAILED);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDConnect);
			QCOMPARE(static_cast<const IfdConnect*>(result.data())->getSlotName(), QStringLiteral("NFC Reader"));

			QCOMPARE(card->disconnect(), CardReturnCode::COMMAND_FAILED);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDDisconnect);
			QCOMPARE(static_cast<const IfdDisconnect*>(result.data())->getSlotHandle(), QStringLiteral("NFC Reader"));

			res = card->transmit(cmd);
			QCOMPARE(res.mReturnCode, CardReturnCode::COMMAND_FAILED);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const RemoteMessage> >(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(static_cast<const IfdTransmit*>(result.data())->getSlotHandle(), QStringLiteral("NFC Reader"));
			QCOMPARE(static_cast<const IfdTransmit*>(result.data())->getInputApdu(), QByteArray("ping"));
			QCOMPARE(res.mResponseApdu.getBuffer(), QByteArray());
		}


		void testUnexpectedMessagesCauseAnIfdErrorMessage()
		{
			QSignalSpy spySend(mDispatcher1.data(), &MockRemoteDispatcher::fireSend);

			mDispatcher1->setState(MockRemoteDispatcher::DispatcherState::WithoutReader);
			Q_EMIT mRemoteClient->fireNewRemoteDispatcher(mDispatcher1);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			spySend.clear();

			const QVector<QSharedPointer<const RemoteMessage> > clientMessages({
						QSharedPointer<const RemoteMessage>(new IfdEstablishContext(IfdVersion::Version::latest, "MAC-MINI")),
						QSharedPointer<const RemoteMessage>(new IfdGetStatus("Remote Reader")),
						QSharedPointer<const RemoteMessage>(new IfdConnect("NFC Reader")),
						QSharedPointer<const RemoteMessage>(new IfdDisconnect("NFC Reader")),
						QSharedPointer<const RemoteMessage>(new IfdTransmit("NFC Reader", "00A402022F00")),
						QSharedPointer<const RemoteMessage>(new IfdEstablishPaceChannel("SlotHandle", EstablishPaceChannel(), 6))
					}
					);
			for (const auto& clientMessage : clientMessages)
			{
				QMetaObject::invokeMethod(mDispatcher1.data(), [this, clientMessage] {
						mDispatcher1->onReceived(clientMessage);
					}, Qt::QueuedConnection);
				QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
				const QList<QVariant>& arguments = spySend.last();

				const QVariant remoteMessageVariant = arguments.at(0);
				QVERIFY(remoteMessageVariant.canConvert<QSharedPointer<const RemoteMessage> >());
				const QSharedPointer<const RemoteMessage> message = remoteMessageVariant.value<QSharedPointer<const RemoteMessage> >();
				const QSharedPointer<const IfdError> errorMessage = message.staticCast<const IfdError>();

				QVERIFY(!errorMessage.isNull());
				QCOMPARE(errorMessage->getType(), RemoteCardMessageType::IFDError);
				QCOMPARE(errorMessage->getContextHandle(), QString());
				QCOMPARE(errorMessage->getSlotHandle(), QString());
				QVERIFY(errorMessage->resultHasError());
				QCOMPARE(errorMessage->getResultMinor(), ECardApiResult::Minor::AL_Unkown_API_Function);

				spySend.clear();
			}
		}


};

QTEST_GUILESS_MAIN(test_RemoteReaderManagerPlugIn)
#include "test_RemoteReaderManagerPlugin.moc"
