/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "FileDestination.h"
#include "ReaderDetector.h"

#include <QFile>
#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_drivers)


static QVector<ReaderType> readerTypesForReaderInfos(const QVector<ReaderInfo>& pReaderInfos)
{
	QVector<ReaderType> readerTypes;
	readerTypes.reserve(pReaderInfos.size());
	for (const auto& readerInfo : pReaderInfos)
	{
		readerTypes += readerInfo.getReaderType();
	}

	return readerTypes;
}


QVector<QSharedPointer<DeviceDescriptor> > ReaderDetector::convertSettings(const QSharedPointer<DriverSettings>& pSettings)
{
	if (pSettings.isNull())
	{
		qCWarning(card_drivers) << "invalid input settings";

		return QVector<QSharedPointer<DeviceDescriptor> >();
	}

	QVector<QSharedPointer<DeviceDescriptor> > devices;
	for (const auto& driver : pSettings->getDrivers())
	{
		const QSharedPointer<DeviceDescriptor> device(new DeviceDescriptor(driver));
		if (device->getReaderType() == ReaderType::UNKNOWN)
		{
			qCWarning(card_drivers) << "invalid reader type in input settings, bailing out";

			return QVector<QSharedPointer<DeviceDescriptor> >();
		}

		devices += device;
	}

	return devices;
}


QSharedPointer<DeviceDescriptor> ReaderDetector::lookupDevice(uint pVendorId, uint pProductId) const
{
	for (const auto& descriptor : mSupportedDevices)
	{
		if (pVendorId == descriptor->getVendorId() && pProductId == descriptor->getProductId())
		{
			return descriptor;
		}
	}

	return QSharedPointer<DeviceDescriptor>();
}


ReaderDetector::ReaderDetector(const QSharedPointer<DriverSettings>& pDriverSettings)
	: mSupportedDevices(convertSettings(pDriverSettings))
#ifdef Q_OS_LINUX
	, mDeviceListener(nullptr)
#endif
{
	qCDebug(card_drivers) << "initNativeEvents() =" << initNativeEvents();
}


ReaderDetector::~ReaderDetector()
{
	qCDebug(card_drivers) << "terminateNativeEvents() =" << terminateNativeEvents();
}


QVector<QSharedPointer<DeviceDescriptor> > ReaderDetector::getAttachedDevices(const QVector<ReaderInfo>& pReaderInfos) const
{
	const auto readerTypes = readerTypesForReaderInfos(pReaderInfos);

	QVector<QSharedPointer<DeviceDescriptor> > result;
	for (const auto& devId : attachedDevIds())
	{
		const uint vendorId = devId.first;
		const uint productId = devId.second;
		const QSharedPointer<DeviceDescriptor> device = lookupDevice(vendorId, productId);
		if (device)
		{
			device->checkDriver(readerTypes);
			result += device;
		}
	}
	return result;
}
