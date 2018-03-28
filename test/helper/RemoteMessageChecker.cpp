/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */


#include "RemoteMessageChecker.h"

#include "messages/Discovery.h"
#include "messages/GetIfdStatus.h"
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

#include <QDebug>
#include <QtTest/QtTest>


using namespace governikus;


void RemoteMessageChecker::processDiscovery(const QSharedPointer<const Discovery>& pMessage)
{
	QCOMPARE(pMessage->getIfdName(), QStringLiteral("Sony Xperia Z5 compact"));
	QCOMPARE(pMessage->getIfdId(), QStringLiteral("0123456789ABCDEF"));
	QVERIFY(pMessage->getPort() == static_cast<quint16>(24728));
	QCOMPARE(pMessage->getSupportedApis(), {IfdVersion::Version::v0});
}


void RemoteMessageChecker::process(const QSharedPointer<const IfdEstablishContext>& pMessage)
{
	QCOMPARE(pMessage->getType(), RemoteCardMessageType::IFDEstablishContext);
	QCOMPARE(pMessage->getProtocol().toString(), QStringLiteral("IFDInterface_WebSocket_v0"));
}


void RemoteMessageChecker::process(const QSharedPointer<const IfdEstablishContextResponse>& pMessage)
{
	QCOMPARE(pMessage->getType(), RemoteCardMessageType::IFDEstablishContextResponse);
	QCOMPARE(pMessage->resultHasError(), true);
	QCOMPARE(pMessage->getResultMinor(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult"));
}


void RemoteMessageChecker::process(const QSharedPointer<const GetIfdStatus>& pMessage)
{
	QCOMPARE(pMessage->getType(), RemoteCardMessageType::IFDGetStatus);
}


void RemoteMessageChecker::process(const QSharedPointer<const IfdStatus>& pMessage)
{
	QCOMPARE(pMessage->getType(), RemoteCardMessageType::IFDStatus);

	QCOMPARE(pMessage->getSlotName(), QStringLiteral("NFC Reader"));
	QVERIFY(pMessage->getConnectedReader());
	QCOMPARE(pMessage->getMaxApduLength(), 500);

	const PaceCapabilities paceCapabilities = pMessage->getPaceCapabilities();
	Q_UNUSED(paceCapabilities);

	QVERIFY(!pMessage->getCardAvailable());
}


void RemoteMessageChecker::process(const QSharedPointer<const IfdConnect>& pMessage)
{
	QCOMPARE(pMessage->getType(), RemoteCardMessageType::IFDConnect);
	QCOMPARE(pMessage->getSlotName(), QStringLiteral("NFC Reader"));
}


void RemoteMessageChecker::process(const QSharedPointer<const IfdConnectResponse>& pMessage)
{
	QCOMPARE(pMessage->getType(), RemoteCardMessageType::IFDConnectResponse);
	QCOMPARE(pMessage->getSlotHandle(), QStringLiteral("NFC Reader"));
	QCOMPARE(pMessage->resultHasError(), true);
	QCOMPARE(pMessage->getResultMinor(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult"));
}


void RemoteMessageChecker::process(const QSharedPointer<const IfdDisconnect>& pMessage)
{
	QCOMPARE(pMessage->getType(), RemoteCardMessageType::IFDDisconnect);
	QCOMPARE(pMessage->getSlotHandle(), QStringLiteral("NFC Reader"));
}


void RemoteMessageChecker::process(const QSharedPointer<const IfdDisconnectResponse>& pMessage)
{
	QCOMPARE(pMessage->getType(), RemoteCardMessageType::IFDDisconnectResponse);
	QCOMPARE(pMessage->getSlotHandle(), QStringLiteral("NFC Reader"));
	QCOMPARE(pMessage->resultHasError(), true);
	QCOMPARE(pMessage->getResultMinor(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult"));
}


void RemoteMessageChecker::process(const QSharedPointer<const IfdError>& pMessage)
{
	QCOMPARE(pMessage->getType(), RemoteCardMessageType::IFDError);
	QCOMPARE(pMessage->getSlotHandle(), QStringLiteral("NFC Reader"));
	QCOMPARE(pMessage->resultHasError(), true);
	QCOMPARE(pMessage->getResultMinor(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult"));
}


void RemoteMessageChecker::process(const QSharedPointer<const IfdTransmit>& pMessage)
{
	QCOMPARE(pMessage->getType(), RemoteCardMessageType::IFDTransmit);
	QCOMPARE(pMessage->getSlotHandle(), QStringLiteral("NFC Reader"));
	QCOMPARE(pMessage->getInputApdu(), QByteArray::fromHex("00A402022F00"));
}


void RemoteMessageChecker::process(const QSharedPointer<const IfdTransmitResponse>& pMessage)
{
	QCOMPARE(pMessage->getType(), RemoteCardMessageType::IFDTransmitResponse);
	QCOMPARE(pMessage->getSlotHandle(), QStringLiteral("NFC Reader"));
	QCOMPARE(pMessage->resultHasError(), true);
	QCOMPARE(pMessage->getResultMinor(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult"));
	QCOMPARE(pMessage->getResponseApdu(), QByteArray::fromHex("9000"));
}


void RemoteMessageChecker::process(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage)
{
	QCOMPARE(pMessage->getSlotHandle(), QStringLiteral("My little Reader"));
	QCOMPARE(pMessage->getInputData(), QByteArray::fromHex("ABCD1234"));
}


void RemoteMessageChecker::process(const QSharedPointer<const IfdEstablishPaceChannelResponse>& pMessage)
{
	QCOMPARE(pMessage->getSlotHandle(), QStringLiteral("My little Reader"));
	QCOMPARE(pMessage->getOutputData(), QByteArray::fromHex("ABCD1234"));
}
