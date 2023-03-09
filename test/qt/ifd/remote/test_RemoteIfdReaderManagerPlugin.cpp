/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref RemoteIfdReaderManagerPlugIn
 */

#include "RemoteIfdReaderManagerPlugIn.h"

#include "AppSettings.h"
#include "Env.h"
#include "MockIfdDispatcher.h"
#include "Reader.h"
#include "RemoteIfdClient.h"
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

#include <QQueue>
#include <QSemaphore>
#include <QtCore>
#include <QtTest>

using namespace governikus;


Q_DECLARE_METATYPE(QSharedPointer<const IfdMessage>)
Q_DECLARE_METATYPE(ReaderInfo)


class MockIfdClient
	: public IfdClient
{
	Q_OBJECT

	public:
		MockIfdClient() = default;
		~MockIfdClient() override = default;

		void startDetection() override;
		void stopDetection() override;
		bool isDetecting() override;
		void establishConnection(const QSharedPointer<IfdListEntry>& pEntry, const QString& pPsk) override;
		QVector<RemoteServiceSettings::RemoteInfo> getConnectedDeviceInfos() override;
};


void MockIfdClient::startDetection()
{
}


void MockIfdClient::stopDetection()
{
}


bool MockIfdClient::isDetecting()
{
	return false;
}


void MockIfdClient::establishConnection(const QSharedPointer<IfdListEntry>& pEntry, const QString& pPsk)
{
	Q_UNUSED(pEntry)
	Q_UNUSED(pPsk)
}


QVector<RemoteServiceSettings::RemoteInfo> MockIfdClient::getConnectedDeviceInfos()
{
	return QVector<RemoteServiceSettings::RemoteInfo>();
}


class test_RemoteIfdReaderManagerPlugIn
	: public QObject
{
	Q_OBJECT

	private:
		QThread mNetworkThread;
		QSharedPointer<MockIfdClient> mIfdClient;
		QSharedPointer<RemoteIfdReaderManagerPlugIn> mPlugin;
		QSharedPointer<MockIfdDispatcher> mDispatcher1;
		QSharedPointer<MockIfdDispatcher> mDispatcher2;
		QVector<QSharedPointer<const IfdMessage>> mClientMessages;

		ReaderInfo getReaderInfo(QSignalSpy& pSpy)
		{
			return qvariant_cast<ReaderInfo>(pSpy.takeFirst().at(0));
		}

	private Q_SLOTS:
		void init()
		{
			mNetworkThread.setObjectName(QStringLiteral("NetworkThread"));
			mNetworkThread.start();

			mIfdClient.reset(new MockIfdClient());
			Env::set(RemoteIfdClient::staticMetaObject, mIfdClient.data());

			mDispatcher1.reset(new MockIfdDispatcher());
			mDispatcher1->moveToThread(&mNetworkThread);

			mDispatcher2.reset(new MockIfdDispatcher());
			mDispatcher2->moveToThread(&mNetworkThread);

			mPlugin.reset(new RemoteIfdReaderManagerPlugIn());
			mPlugin->init();
		}


		void cleanup()
		{
			mIfdClient.reset();
			mDispatcher1.reset();
			mDispatcher2.reset();
			mPlugin.reset();

			mNetworkThread.quit();
			mNetworkThread.wait();
		}


		void testStandardValues()
		{
			QSignalSpy spySend(mDispatcher1.data(), &MockIfdDispatcher::fireSend);

			QVERIFY(mPlugin->getReaders().isEmpty());

			Q_EMIT mIfdClient->fireNewDispatcher(mDispatcher1);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations

			QVERIFY(mPlugin->getReaders().isEmpty());
			QCOMPARE(spySend.size(), 1);

			QSharedPointer<const IfdMessage> result = qvariant_cast<QSharedPointer<const IfdMessage>>(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), IfdMessageType::IFDEstablishContext);
		}


		void testSingleDispatcherSingleReaderAddRemoveWithoutCard()
		{
			QSignalSpy spySend(mDispatcher1.data(), &MockIfdDispatcher::fireSend);

			Q_EMIT mIfdClient->fireNewDispatcher(mDispatcher1);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			spySend.clear();

			QSharedPointer<IfdStatus> message;
			QSignalSpy spyAdded(mPlugin.data(), &ReaderManagerPlugIn::fireReaderAdded);
			QSignalSpy spyRemoved(mPlugin.data(), &ReaderManagerPlugIn::fireReaderRemoved);

			ReaderInfo info(QStringLiteral("NFC Reader"), ReaderManagerPlugInType::NFC);
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

			message.reset(new IfdStatus(info));
			message->mConnectedReader = false;
			mDispatcher1->onReceived(message);
			QCOMPARE(mPlugin->getReaders().size(), 0);
			QCOMPARE(spySend.size(), 0);
			QCOMPARE(spyAdded.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(getReaderInfo(spyRemoved).getName(), QStringLiteral("NFC Reader#TestContext"));

			message.reset(new IfdStatus(info));
			message->mConnectedReader = true;
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
			QSignalSpy spySend(mDispatcher1.data(), &MockIfdDispatcher::fireSend);

			Q_EMIT mIfdClient->fireNewDispatcher(mDispatcher1);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			spySend.clear();

			QSharedPointer<IfdMessage> message;
			QSignalSpy spyAdded(mPlugin.data(), &ReaderManagerPlugIn::fireReaderAdded);
			QSignalSpy spyRemoved(mPlugin.data(), &ReaderManagerPlugIn::fireReaderRemoved);

			ReaderInfo info(QStringLiteral("NFC Reader"), ReaderManagerPlugInType::NFC);
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
			QSignalSpy spySend1(mDispatcher1.data(), &MockIfdDispatcher::fireSend);
			QSignalSpy spySend2(mDispatcher2.data(), &MockIfdDispatcher::fireSend);

			Q_EMIT mIfdClient->fireNewDispatcher(mDispatcher1);
			Q_EMIT mIfdClient->fireNewDispatcher(mDispatcher2);
			QTRY_COMPARE(spySend1.count(), 1); // clazy:exclude=qstring-allocations
			QTRY_COMPARE(spySend2.count(), 1); // clazy:exclude=qstring-allocations
			spySend1.clear();
			spySend2.clear();

			QSharedPointer<IfdStatus> message;
			QSignalSpy spyAdded(mPlugin.data(), &ReaderManagerPlugIn::fireReaderAdded);
			QSignalSpy spyRemoved(mPlugin.data(), &ReaderManagerPlugIn::fireReaderRemoved);

			ReaderInfo info1(QStringLiteral("NFC Reader 1"), ReaderManagerPlugInType::NFC);
			info1.setMaxApduLength(500);
			info1.setBasicReader(true);
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(false);
			message.reset(new IfdStatus(info1));
			mDispatcher1->onReceived(message);

			ReaderInfo info2(QStringLiteral("NFC Reader 2"), ReaderManagerPlugInType::NFC);
			info2.setMaxApduLength(500);
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

			message.reset(new IfdStatus(info1));
			message->mConnectedReader = false;
			mDispatcher1->onReceived(message);

			QCOMPARE(spySend1.size(), 0);
			QCOMPARE(spySend2.size(), 0);
			QCOMPARE(spyAdded.size(), 0);
			QCOMPARE(spyRemoved.size(), 1);
			QCOMPARE(getReaderInfo(spyRemoved).getName(), QStringLiteral("NFC Reader 1#TestContext"));
			QCOMPARE(mPlugin->getReaders().size(), 1);
			QCOMPARE(mPlugin->getReaders().at(0)->getName(), QStringLiteral("NFC Reader 2#TestContext"));

			message.reset(new IfdStatus(info1));
			message->mConnectedReader = true;
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
			QSharedPointer<const IfdMessage> result;
			QSignalSpy spySend(mDispatcher1.data(), &MockIfdDispatcher::fireSend);

			mDispatcher1->setState(MockIfdDispatcher::DispatcherState::ReaderWithCardError);
			Q_EMIT mIfdClient->fireNewDispatcher(mDispatcher1);

			QTRY_COMPARE(spySend.count(), 2); // clazy:exclude=qstring-allocations
			spySend.takeFirst();
			result = qvariant_cast<QSharedPointer<const IfdMessage>>(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), IfdMessageType::IFDConnect);

			QSharedPointer<IfdMessage> message;

			ReaderInfo info1(QStringLiteral("NFC Reader"), ReaderManagerPlugInType::NFC);
			info1.setMaxApduLength(500);
			message.reset(new IfdStatus(info1));
			info1.setBasicReader(true);
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(false);
			mDispatcher1->onReceived(message);
			QCOMPARE(mPlugin->getReaders().at(0)->getCard(), nullptr);

			QSignalSpy spyInserted(mPlugin->getReaders().at(0), &Reader::fireCardInserted);
			QSignalSpy spyRemoved(mPlugin->getReaders().at(0), &Reader::fireCardRemoved);
			QSignalSpy spyChanged(mPlugin->getReaders().at(0), &Reader::fireCardInfoChanged);
			QSignalSpy spyUpdated(mPlugin->getReaders().at(0), &Reader::fireReaderPropertiesUpdated);

			info1.setCardInfo(CardInfo(CardType::EID_CARD));
			message.reset(new IfdStatus(info1));
			mDispatcher1->onReceived(message);

			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const IfdMessage>>(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), IfdMessageType::IFDConnect);
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

			ReaderInfo info2(QStringLiteral("NFC Reader"), ReaderManagerPlugInType::NFC);
			info2.setMaxApduLength(1);
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
			result = qvariant_cast<QSharedPointer<const IfdMessage>>(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), IfdMessageType::IFDConnect);
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
			QSharedPointer<const IfdMessage> result;
			QSignalSpy spySend(mDispatcher1.data(), &MockIfdDispatcher::fireSend);

			mDispatcher1->setState(MockIfdDispatcher::DispatcherState::ReaderWithCard);
			Q_EMIT mIfdClient->fireNewDispatcher(mDispatcher1);

			QTRY_COMPARE(spySend.count(), 8); // clazy:exclude=qstring-allocations
			spySend.takeFirst();
			result = qvariant_cast<QSharedPointer<const IfdMessage>>(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), IfdMessageType::IFDConnect);
			for (int i = 0; i < 5; ++i) // CardInfo detection
			{
				result = qvariant_cast<QSharedPointer<const IfdMessage>>(spySend.takeFirst().at(0));
				QCOMPARE(result->getType(), IfdMessageType::IFDTransmit);
			}
			result = qvariant_cast<QSharedPointer<const IfdMessage>>(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), IfdMessageType::IFDDisconnect);

			QCOMPARE(mPlugin->getReaders().size(), 1);
			Card* card = mPlugin->getReaders().at(0)->getCard();
			QVERIFY(card != nullptr);

			QCOMPARE(card->establishConnection(), CardReturnCode::OK);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const IfdMessage>>(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), IfdMessageType::IFDConnect);
			QCOMPARE(static_cast<const IfdConnect*>(result.data())->getSlotName(), QStringLiteral("NFC Reader"));

			QCOMPARE(card->releaseConnection(), CardReturnCode::OK);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const IfdMessage>>(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), IfdMessageType::IFDDisconnect);
			QCOMPARE(static_cast<const IfdDisconnect*>(result.data())->getSlotHandle(), QStringLiteral("NFC Reader"));

			CommandApdu cmd(QByteArray("ping"));
			ResponseApduResult res = card->transmit(cmd);
			QCOMPARE(res.mReturnCode, CardReturnCode::OK);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const IfdMessage>>(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), IfdMessageType::IFDTransmit);
			QCOMPARE(static_cast<const IfdTransmit*>(result.data())->getSlotHandle(), QStringLiteral("NFC Reader"));
			QCOMPARE(static_cast<const IfdTransmit*>(result.data())->getInputApdu(), QByteArray("ping"));
			QCOMPARE(res.mResponseApdu, QByteArray("pong"));

			mDispatcher1->setState(MockIfdDispatcher::DispatcherState::ReaderWithCardError);

			QCOMPARE(card->establishConnection(), CardReturnCode::COMMAND_FAILED);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const IfdMessage>>(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), IfdMessageType::IFDConnect);
			QCOMPARE(static_cast<const IfdConnect*>(result.data())->getSlotName(), QStringLiteral("NFC Reader"));

			QCOMPARE(card->releaseConnection(), CardReturnCode::COMMAND_FAILED);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const IfdMessage>>(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), IfdMessageType::IFDDisconnect);
			QCOMPARE(static_cast<const IfdDisconnect*>(result.data())->getSlotHandle(), QStringLiteral("NFC Reader"));

			res = card->transmit(cmd);
			QCOMPARE(res.mReturnCode, CardReturnCode::COMMAND_FAILED);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			result = qvariant_cast<QSharedPointer<const IfdMessage>>(spySend.takeFirst().at(0));
			QCOMPARE(result->getType(), IfdMessageType::IFDTransmit);
			QCOMPARE(static_cast<const IfdTransmit*>(result.data())->getSlotHandle(), QStringLiteral("NFC Reader"));
			QCOMPARE(static_cast<const IfdTransmit*>(result.data())->getInputApdu(), QByteArray("ping"));
			QCOMPARE(res.mResponseApdu, QByteArray());
		}


		void testUnexpectedMessagesCauseAnIfdErrorMessage()
		{
			QSignalSpy spySend(mDispatcher1.data(), &MockIfdDispatcher::fireSend);

			mDispatcher1->setState(MockIfdDispatcher::DispatcherState::WithoutReader);
			Q_EMIT mIfdClient->fireNewDispatcher(mDispatcher1);
			QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
			spySend.clear();

			const QVector<QSharedPointer<const IfdMessage>> clientMessages({
						QSharedPointer<const IfdMessage>(new IfdEstablishContext(IfdVersion::Version::latest, "MAC-MINI")),
						QSharedPointer<const IfdMessage>(new IfdGetStatus("Remote Reader")),
						QSharedPointer<const IfdMessage>(new IfdConnect("NFC Reader")),
						QSharedPointer<const IfdMessage>(new IfdDisconnect("NFC Reader")),
						QSharedPointer<const IfdMessage>(new IfdTransmit("NFC Reader", "00A402022F00")),
						QSharedPointer<const IfdMessage>(new IfdEstablishPaceChannel("SlotHandle", EstablishPaceChannel(), 6))
					}
					);
			for (const auto& clientMessage : clientMessages)
			{
				QMetaObject::invokeMethod(mDispatcher1.data(), [this, clientMessage] {
						mDispatcher1->onReceived(clientMessage);
					}, Qt::QueuedConnection);
				QTRY_COMPARE(spySend.count(), 1); // clazy:exclude=qstring-allocations
				const QList<QVariant>& arguments = spySend.last();

				const QVariant ifdMessageVariant = arguments.at(0);
				QVERIFY(ifdMessageVariant.canConvert<QSharedPointer<const IfdMessage>>());
				const QSharedPointer<const IfdMessage> message = ifdMessageVariant.value<QSharedPointer<const IfdMessage>>();
				const QSharedPointer<const IfdError> errorMessage = message.staticCast<const IfdError>();

				QVERIFY(!errorMessage.isNull());
				QCOMPARE(errorMessage->getType(), IfdMessageType::IFDError);
				QCOMPARE(errorMessage->getContextHandle(), QString());
				QCOMPARE(errorMessage->getSlotHandle(), QString());
				QVERIFY(errorMessage->resultHasError());
				QCOMPARE(errorMessage->getResultMinor(), ECardApiResult::Minor::AL_Unknown_API_Function);

				spySend.clear();
			}
		}


};

QTEST_GUILESS_MAIN(test_RemoteIfdReaderManagerPlugIn)
#include "test_RemoteIfdReaderManagerPlugin.moc"
