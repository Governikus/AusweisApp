/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "WifiInfo.h"

#include <QNetworkInterface>


using namespace governikus;


bool WifiInfo::isPrivateIp(const QHostAddress& pAddress)
{
	return !pAddress.isNull() &&
		   (
		pAddress.isInSubnet(QHostAddress::parseSubnet(QStringLiteral("10.0.0.0/8"))) ||
		pAddress.isInSubnet(QHostAddress::parseSubnet(QStringLiteral("172.16.0.0/12"))) ||
		pAddress.isInSubnet(QHostAddress::parseSubnet(QStringLiteral("192.168.0.0/16"))) ||
		pAddress.isInSubnet(QHostAddress::parseSubnet(QStringLiteral("fd00::/8")))
		   );
}


bool WifiInfo::hasPrivateIpAddress()
{
	const auto& interfaces = QNetworkInterface::allInterfaces();
	for (const QNetworkInterface& interface : interfaces)
	{
		const auto& entries = interface.addressEntries();
		for (const QNetworkAddressEntry& addressEntry : entries)
		{
			if (isPrivateIp(addressEntry.ip()))
			{
				return true;
			}
		}
	}

	return false;
}
