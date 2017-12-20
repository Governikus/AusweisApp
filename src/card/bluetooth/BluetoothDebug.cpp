/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothDebug.h"

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QMetaEnum>
#include <QMetaObject>

namespace
{

QString toString(QBluetoothDeviceInfo::CoreConfigurations pConfig)
{
	switch (pConfig)
	{
		case QBluetoothDeviceInfo::CoreConfiguration::BaseRateAndLowEnergyCoreConfiguration:
			return QStringLiteral("BaseRateAndLowEnergyCoreConfiguration");

		case QBluetoothDeviceInfo::CoreConfiguration::BaseRateCoreConfiguration:
			return QStringLiteral("BaseRateCoreConfiguration");

		case QBluetoothDeviceInfo::CoreConfiguration::LowEnergyCoreConfiguration:
			return QStringLiteral("LowEnergyCoreConfiguration");

		case QBluetoothDeviceInfo::CoreConfiguration::UnknownCoreConfiguration:
			return QStringLiteral("UnknownCoreConfiguration");
	}

	return QStringLiteral("unknown QBluetoothDeviceInfo::CoreConfiguration(%1)").arg(static_cast<int>(pConfig));
}


}


QDebug operator <<(QDebug pDbg, const QBluetoothDeviceInfo::CoreConfigurations& pConfig)
{
	QDebugStateSaver saver(pDbg);
	return pDbg.nospace() << toString(pConfig);
}


QDebug operator <<(QDebug pDbg, const QBluetoothDeviceInfo& pInfo)
{
	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << QStringLiteral("%1 (%2, %3.%4, %5)").arg(
			pInfo.name(),
			pInfo.address().isNull() ? pInfo.deviceUuid().toString() : pInfo.address().toString(),
			QString::number(pInfo.majorDeviceClass()),
			QString::number(pInfo.minorDeviceClass()),
			toString(pInfo.coreConfigurations()));
	return pDbg;
}
