/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderDetector.h"

#include <windows.h>

#include <dbt.h>
#include <setupapi.h>

// Defines MAX_DEVICE_ID_LEN
#include <cfgmgr32.h>

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QStringList>


Q_DECLARE_LOGGING_CATEGORY(card_drivers)

using namespace governikus;

bool ReaderDetector::initNativeEvents()
{
	QCoreApplication::instance()->installNativeEventFilter(this);
	return true;
}


bool ReaderDetector::terminateNativeEvents()
{
	QCoreApplication::instance()->removeNativeEventFilter(this);
	return true;
}


static QStringList attachedDevStringIds()
{
	QStringList result;
	SP_DEVINFO_DATA deviceInfoData;

	// Find devices
	HDEVINFO deviceInfoSet = SetupDiGetClassDevs(/* ClassGuid */ nullptr,
			/* Enumerator */ nullptr,
			/* hwndParent */ nullptr,
			/* Flags */ DIGCF_ALLCLASSES | DIGCF_PRESENT);

	if (deviceInfoSet == INVALID_HANDLE_VALUE)
	{
		qCWarning(card_drivers) << "devs contains an invalid handle value, exiting";

		return QStringList();
	}

	deviceInfoData.cbSize = sizeof(deviceInfoData);
	for (DWORD deviceIndex = 0; SetupDiEnumDeviceInfo(deviceInfoSet, deviceIndex, &deviceInfoData); deviceIndex++)
	{
		//
		// determine instance ID
		//
		TCHAR devId[MAX_DEVICE_ID_LEN];
		if (CM_Get_Device_ID(deviceInfoData.DevInst, devId, MAX_DEVICE_ID_LEN, 0) != CR_SUCCESS)
		{
			devId[0] = TEXT('\0');
		}

		result += QString::fromWCharArray(devId);
	}

	SetupDiDestroyDeviceInfoList(deviceInfoSet);
	return result;
}


/**
 * \brief Get an id (vendor or product) from a device identifier in the format
 *        returned by CM_Get_Device_ID. If the format is not correct, return 0x0.
 */
static uint getId(const QString& pDevId, const QString& pPrefix)
{
	static const int ID_LENGTH = 4;
	const int prefixPosition = pDevId.indexOf(pPrefix, 0, Qt::CaseInsensitive);
	if (prefixPosition == -1 || pDevId.size() < prefixPosition + pPrefix.size() + ID_LENGTH)
	{
		return 0;
	}
	else
	{
		const QString idStr = pDevId.mid(prefixPosition + pPrefix.size(), ID_LENGTH);
		bool ok = false;
		const uint id = idStr.toUInt(&ok, 16);
		return ok ? id : 0;
	}
}


// Look for vendor Id. If not found, return 0x0.
static uint getVendorId(const QString& pDevId)
{
	return getId(pDevId, QStringLiteral("VID_"));
}


// Look for product Id. If not found, return 0x0.
static uint getProductId(const QString& pDevId)
{
	return getId(pDevId, QStringLiteral("PID_"));
}


QVector<UsbId> ReaderDetector::attachedDevIds() const
{
	QVector<UsbId> result;

	for (auto stringDevId : attachedDevStringIds())
	{
		const uint vendorId = getVendorId(stringDevId);
		const uint productId = getProductId(stringDevId);
		if (vendorId != 0 && productId != 0)
		{
			result += UsbId(vendorId, productId);
		}
	}

	return result;
}


bool ReaderDetector::nativeEventFilter(const QByteArray& pEventType, void* pMessage, long* pResult)
{
	Q_UNUSED(pResult)

	if (pEventType == "windows_generic_MSG")
	{
		MSG* msg = reinterpret_cast<MSG*>(pMessage);
		if (msg->message == WM_DEVICECHANGE)
		{
			if (msg->wParam == DBT_DEVNODES_CHANGED)
			{
				qCDebug(card_drivers) << "System information: device changed";
				Q_EMIT fireReaderChangeDetected();
			}
		}
	}

	return false;
}
