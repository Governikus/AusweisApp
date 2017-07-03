/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */


#include "RemoteCardNotificationChecker.h"

#include <QtTest/QtTest>

#include <QDebug>

using namespace governikus;


void RemoteCardNotificationChecker::process(const QSharedPointer<RemoteReaderDiscoverCmd>& pNotification)
{
	QCOMPARE(pNotification->getType(), CardNotificationType::REMOTE_READER_DISCOVER);
}


void RemoteCardNotificationChecker::process(const QSharedPointer<RemoteReaderOfferMsg>& pNotification)
{
	QCOMPARE(pNotification->getType(), CardNotificationType::REMOTE_READER_OFFER);
	QCOMPARE(pNotification->getDeviceName(), QStringLiteral("Sony Xperia Z5 compact"));
	QVERIFY(pNotification->isEncrypted());
	QVERIFY(pNotification->getPort() == static_cast<quint16>(24728));
	QCOMPARE(pNotification->getAvailableApiLevels(), QVector<int>({1, 2, 3, 4}));
}


void RemoteCardNotificationChecker::process(const QSharedPointer<GetApiLevelCmd>& pNotification)
{
	QCOMPARE(pNotification->getType(), CardNotificationType::GET_API_LEVEL);
}


void RemoteCardNotificationChecker::process(const QSharedPointer<SetApiLevelCmd>& pNotification)
{
	QCOMPARE(pNotification->getType(), CardNotificationType::SET_API_LEVEL);
	QCOMPARE(pNotification->getLevel(), 1);
}


void RemoteCardNotificationChecker::process(const QSharedPointer<ApiLevelMsg>& pNotification)
{
	QCOMPARE(pNotification->getType(), CardNotificationType::API_LEVEL);
	QCOMPARE(pNotification->getError(), QStringLiteral("No error"));
	QCOMPARE(pNotification->getAvailable(), QVector<int>({1, 2, 3, 4}));
	QVERIFY(pNotification->getCurrent() == 2);
}


void RemoteCardNotificationChecker::process(const QSharedPointer<GetReaderListCmd>& pNotification)
{
	QCOMPARE(pNotification->getType(), CardNotificationType::GET_READER_LIST);
}


void RemoteCardNotificationChecker::process(const QSharedPointer<ReaderListMsg>& pNotification)
{
	QCOMPARE(pNotification->getType(), CardNotificationType::READER_LIST);

	// Readers.
	const QVector<ReaderDescription>& readers = pNotification->getReaders();
	QCOMPARE(readers.size(), 1);
	const ReaderDescription& reader = readers.first();
	QCOMPARE(reader.getName(), QStringLiteral("NFC Reader"));
	QVERIFY(reader.isAttached());
	QCOMPARE(reader.getExtendedLength(), ExtendedLengthApduSupportCode::SUPPORTED);

	// Cards.
	QVERIFY(reader.hasCard());

	const CardDescription& card = reader.getCard();
	QCOMPARE(card.getEFCardAccess(), QByteArray::fromHex("3181c10000"));
	QCOMPARE(card.getRetryCounter(), 3);
	QVERIFY(!card.isPinDeactivated());
	QVERIFY(!card.isConnected());
}


void RemoteCardNotificationChecker::process(const QSharedPointer<ReaderMsg>& pNotification)
{
	QCOMPARE(pNotification->getType(), CardNotificationType::READER);
	QCOMPARE(pNotification->getError(), QStringLiteral("Optional error message"));

	// Readers.
	const ReaderDescription& reader = pNotification->getReader();
	QCOMPARE(reader.getName(), QStringLiteral("NFC Reader"));
	QVERIFY(!reader.isAttached());
	QCOMPARE(reader.getExtendedLength(), ExtendedLengthApduSupportCode::SUPPORTED);

	// Cards.
	QVERIFY(!reader.hasCard());

	const CardDescription& card = reader.getCard();
	QCOMPARE(card.getEFCardAccess(), QByteArray());
	QCOMPARE(card.getRetryCounter(), -1);
	QVERIFY(card.isPinDeactivated());
	QVERIFY(!card.isConnected());
}


void RemoteCardNotificationChecker::process(const QSharedPointer<ConnectCmd>& pNotification)
{
	QCOMPARE(pNotification->getType(), CardNotificationType::CONNECT);
	QCOMPARE(pNotification->getReaderName(), QStringLiteral("NFC Reader"));
}


void RemoteCardNotificationChecker::process(const QSharedPointer<DisconnectCmd>& pNotification)
{
	QCOMPARE(pNotification->getType(), CardNotificationType::DISCONNECT);
	QCOMPARE(pNotification->getReaderName(), QStringLiteral("NFC Reader"));
}


void RemoteCardNotificationChecker::process(const QSharedPointer<TransmitCmd>& pNotification)
{
	QCOMPARE(pNotification->getType(), CardNotificationType::TRANSMIT);
	QCOMPARE(pNotification->getReaderName(), QStringLiteral("NFC Reader"));
	QCOMPARE(pNotification->getCommandApdu(), QByteArray::fromHex("00A402022F00"));
}


void RemoteCardNotificationChecker::process(const QSharedPointer<TransmitResponseMsg>& pNotification)
{
	QCOMPARE(pNotification->getType(), CardNotificationType::TRANSMIT_RESPONSE);
	QCOMPARE(pNotification->getReaderName(), QStringLiteral("NFC Reader"));
	QCOMPARE(pNotification->getError(), QStringLiteral("Optional error message"));
	QCOMPARE(pNotification->getResponseApdu(), QByteArray::fromHex("9000"));
}


void RemoteCardNotificationChecker::process(const QSharedPointer<UnsupportedMsg>& pNotification)
{
	QCOMPARE(pNotification->getType(), CardNotificationType::UNSUPPORTED);
	QCOMPARE(pNotification->getError(), QStringLiteral("Invalid command"));
}
