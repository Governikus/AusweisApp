/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderDetector.h"

#include <CoreFoundation/CFNumber.h>

#include <IOKit/IOKitKeys.h>
#include <IOKit/usb/IOUSBLib.h>

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(card_drivers)

using namespace governikus;


#define VENDOR_ID "idVendor"
#define PRODUCT_ID "idProduct"


static void deviceChanged(void* refCon, io_iterator_t iterator)
{
	while (IOIteratorNext(iterator))
	{
		// Clear the recognized changes
	}

	qCDebug(card_drivers) << "System information: device changed";

	ReaderDetector* readerDetector = static_cast<ReaderDetector*>(refCon);
	Q_EMIT readerDetector->fireReaderChangeDetected();
}


static bool listenTo(const io_name_t notificationType, ReaderDetector* readerDetector, io_iterator_t* iterator)
{
	//https://developer.apple.com/library/mac/documentation/DeviceDrivers/Conceptual/AccessingHardware/AH_Finding_Devices/AH_Finding_Devices.html

	CFMutableDictionaryRef matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
	IONotificationPortRef notificationObject = IONotificationPortCreate(kIOMasterPortDefault);
	CFRunLoopSourceRef notificationRunLoopSource = IONotificationPortGetRunLoopSource(notificationObject);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), notificationRunLoopSource, kCFRunLoopDefaultMode);
	kern_return_t kr = IOServiceAddMatchingNotification(
			notificationObject,
			notificationType,
			matchingDict,
			deviceChanged,
			readerDetector,
			iterator);

	if (kr != KERN_SUCCESS)
	{
		qCDebug(card_drivers) << "IOServiceAddMatchingNotification returned" << kr;
		return false;
	}

	while (IOIteratorNext(*iterator))
	{
		// Clear already known devices and arm the notification mechanism
	}

	return true;
}


bool ReaderDetector::initNativeEvents()
{
	bool succeed = true;
	succeed &= listenTo(kIOPublishNotification, this, &mIteratorPublish);
	succeed &= listenTo(kIOTerminatedNotification, this, &mIteratorTerminated);

	return succeed;
}


bool ReaderDetector::terminateNativeEvents()
{
	bool succeed = true;
	succeed &= (IOObjectRelease(mIteratorPublish) == KERN_SUCCESS);
	succeed &= (IOObjectRelease(mIteratorTerminated) == KERN_SUCCESS);

	return succeed;
}


static bool getUintValueFromDeviceRegistryEntry(io_object_t pDevice, CFStringRef pKey, uint* pResultValue)
{
	static const CFTypeID NUMBER_TYPE_ID = CFNumberGetTypeID();

	CFTypeRef property = IORegistryEntryCreateCFProperty(pDevice, pKey, kCFAllocatorDefault, /* options */ 0);
	if (property == nullptr)
	{
		qCWarning(card_drivers) << "property" << pKey << "not found in registry entry";

		return false;
	}

	if (CFGetTypeID(property) != NUMBER_TYPE_ID)
	{
		qCWarning(card_drivers) << "property" << pKey << "is not of a numeric type";

		return false;
	}

	CFNumberRef number = static_cast<CFNumberRef>(property);
	if (CFNumberGetType(number) != kCFNumberSInt32Type)
	{
		qCWarning(card_drivers) << "property" << pKey << "has the wrong numeric type";

		return false;
	}

	const bool success = CFNumberGetValue(number, kCFNumberSInt32Type, pResultValue);
	CFRelease(property);
	if (!success)
	{
		qCWarning(card_drivers) << "error while reading numeric value from property" << pKey;
	}

	return success;
}


static UsbId getDeviceIds(io_object_t pDevice)
{
	uint vendorId = 0x0;
	uint productId = 0x0;

	if (!getUintValueFromDeviceRegistryEntry(pDevice, CFSTR(VENDOR_ID), &vendorId))
	{
		qCDebug(card_drivers) << "missing or invalid vendor id";

		return UsbId(0x0, 0x0);
	}

	if (!getUintValueFromDeviceRegistryEntry(pDevice, CFSTR(PRODUCT_ID), &productId))
	{
		qCDebug(card_drivers) << "missing or invalid product id";

		return UsbId(0x0, 0x0);
	}

	return UsbId(vendorId, productId);
}


QVector<UsbId> ReaderDetector::attachedDevIds() const
{
	QVector<UsbId> result;
	mach_port_t myMasterPort = kIOMasterPortDefault;
	io_iterator_t deviceIterator = 0;
	io_object_t currentDevice = 0;

	kern_return_t createIteratorResult = IORegistryCreateIterator(myMasterPort, kIOUSBPlane, kIORegistryIterateRecursively, &deviceIterator);
	if (createIteratorResult != kIOReturnSuccess)
	{
		qCWarning(card_drivers) << "creation of device iterator failed, exiting";

		return QVector<UsbId>();
	}

	currentDevice = IOIteratorNext(deviceIterator);
	while (currentDevice != 0)
	{
		UsbId ids = getDeviceIds(currentDevice);
		if (ids.getVendorId() != 0 && ids.getProductId() != 0)
		{
			result += ids;
		}

		IOObjectRelease(currentDevice);
		currentDevice = IOIteratorNext(deviceIterator);
	}

	IOObjectRelease(deviceIterator);

	return result;
}
