/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

#include "DatagramHandler.h"

#include "Initializer.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(network)


INIT_FUNCTION([] {
			qRegisterMetaType<QHostAddress>("QHostAddress");
		})


bool DatagramHandler::isValidBroadcastInterface(const QNetworkInterface& pInterface)
{
	if (!pInterface.isValid())
	{
		return false;
	}

	const auto& flags = pInterface.flags();
	if (!flags.testFlag(QNetworkInterface::IsUp) || !flags.testFlag(QNetworkInterface::IsRunning))
	{
		return false;
	}

#ifdef Q_OS_MACOS
	// Excluding not documented interface of the T2 Coprocessor on macOS,  which does not accept broadcasts.
	// https://duo.com/labs/research/apple-t2-xpc
	if (pInterface.hardwareAddress().toLower() == QLatin1String("ac:de:48:00:11:22"))
	{
		return false;
	}
#endif

	return true;
}


bool DatagramHandler::isValidAddressEntry(const QNetworkAddressEntry& pEntry)
{
	const auto ipAddr = pEntry.ip();
	switch (ipAddr.protocol())
	{
		case QAbstractSocket::NetworkLayerProtocol::IPv4Protocol:
			return ipAddr.isGlobal() && !pEntry.broadcast().isNull();

		case QAbstractSocket::NetworkLayerProtocol::IPv6Protocol:
			return ipAddr.isGlobal();

		default:
			qCDebug(network) << "Skipping unknown protocol type:" << ipAddr.protocol();
			return false;

	}
}


QList<QNetworkAddressEntry> DatagramHandler::getAllBroadcastEntries()
{
	QList<QNetworkAddressEntry> broadcastEntries;

	const auto& allInterfaces = QNetworkInterface::allInterfaces();
	for (const QNetworkInterface& interface : allInterfaces)
	{
		if (!isValidBroadcastInterface(interface))
		{
			continue;
		}

		const auto& entries = interface.addressEntries();
		for (const QNetworkAddressEntry& addressEntry : entries)
		{
			if (isValidAddressEntry(addressEntry))
			{
				broadcastEntries << addressEntry;
			}
		}
	}

	return broadcastEntries;
}


QHostAddress DatagramHandler::getBroadcastAddress(const QNetworkAddressEntry& pEntry)
{
	const auto& ipAddr = pEntry.ip();
	switch (ipAddr.protocol())
	{
		case QAbstractSocket::NetworkLayerProtocol::IPv4Protocol:
		{
			return pEntry.broadcast();
		}

		case QAbstractSocket::NetworkLayerProtocol::IPv6Protocol:
		{
			return QHostAddress(ipv6MulticastAddress);
		}

		default:
		{
			qCDebug(network) << "Skipping unknown protocol type:" << ipAddr.protocol();
			return QHostAddress();
		}
	}
}


DatagramHandler::DatagramHandler(bool)
{
}
