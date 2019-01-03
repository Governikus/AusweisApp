/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "UsbId.h"

#include <QDebug>
#include <QDebugStateSaver>

using namespace governikus;

UsbId::UsbId(unsigned int pVendorId, unsigned int pProductId)
	: mVendorId(pVendorId)
	, mProductId(pProductId)
{
}


unsigned int UsbId::getVendorId() const
{
	return mVendorId;
}


unsigned int UsbId::getProductId() const
{
	return mProductId;
}


bool UsbId::operator==(const UsbId& pOther) const
{
	return mVendorId == pOther.mVendorId
		   && mProductId == pOther.mProductId;
}


QDebug operator <<(QDebug pDbg, const UsbId& pUsbId)
{
	QDebugStateSaver saver(pDbg);
	pDbg << "USB Vendor ID:" << QStringLiteral("0x%1").arg(pUsbId.getVendorId(), 0, 16) <<
		"USB Product ID:" << QStringLiteral("0x%1").arg(pUsbId.getProductId(), 0, 16);
	return pDbg;
}
