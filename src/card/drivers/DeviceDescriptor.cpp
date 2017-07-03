/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "DeviceDescriptor.h"

using namespace governikus;


DeviceDescriptor::DeviceDescriptor(const QSharedPointer<const Driver>& pDriver)
	: mDriver(pDriver)
	, mReaderType(pDriver.isNull() ?
			ReaderType::UNKNOWN :
			Enum<ReaderType>::fromString(pDriver->getReaderType(), ReaderType::UNKNOWN))
	, mHasDriver(false)
{
}


DeviceDescriptor::~DeviceDescriptor()
{
}


void DeviceDescriptor::checkDriver(const QVector<ReaderType>& pReaderTypes)
{
	mHasDriver = pReaderTypes.contains(mReaderType);
}


ReaderType DeviceDescriptor::getReaderType() const
{
	return mReaderType;
}


uint DeviceDescriptor::getVendorId() const
{
	return mReaderType == ReaderType::UNKNOWN ? 0 : mDriver->getVendorId();
}


uint DeviceDescriptor::getProductId() const
{
	return mReaderType == ReaderType::UNKNOWN ? 0 : mDriver->getProductId();
}


QString DeviceDescriptor::getName() const
{
	return mReaderType == ReaderType::UNKNOWN ? QString() : mDriver->getName();
}


QString DeviceDescriptor::getDriverUrl() const
{
	return mReaderType == ReaderType::UNKNOWN ? QString() : mDriver->getUrl();
}


bool DeviceDescriptor::hasDriver() const
{
	return mHasDriver;
}
