/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "IfdDescriptor.h"

#include "Initializer.h"


using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<IfdDescriptor>("IfdDescriptor");
		})


IfdDescriptor::IfdDescriptorData::IfdDescriptorData(const Discovery& pDiscovery,
		bool pIsLocalIfd)
	: mDiscovery(pDiscovery)
	, mIsLocalIfd(pIsLocalIfd)
{
}


IfdDescriptor::IfdDescriptorData::~IfdDescriptorData() = default;


bool IfdDescriptor::IfdDescriptorData::isSameIfd(const IfdDescriptorData& pOther) const
{
	return mDiscovery.getIfdId() == pOther.mDiscovery.getIfdId();
}


IfdDescriptor::IfdDescriptor(const Discovery& pDiscovery, bool pLocalIfd)
	: d()
{
	if (pDiscovery.getAddresses().isEmpty())
	{
		return;
	}

	d = new IfdDescriptorData(pDiscovery, pLocalIfd);
}


const QString& IfdDescriptor::getIfdName() const
{
	static const QString EMPTY_STRING;

	return isNull() ? EMPTY_STRING : d->mDiscovery.getIfdName();
}


const QByteArray& IfdDescriptor::getIfdId() const
{
	static const QByteArray EMPTY_ARRAY;

	return isNull() ? EMPTY_ARRAY : d->mDiscovery.getIfdId();
}


const QList<IfdVersion::Version>& IfdDescriptor::getSupportedApis() const
{
	static const QList<IfdVersion::Version> EMPTY_VECTOR;

	return isNull() ? EMPTY_VECTOR : d->mDiscovery.getSupportedApis();
}


bool IfdDescriptor::isSupported() const
{
	return IfdVersion(IfdVersion::selectLatestSupported(getSupportedApis())).isValid();
}


bool IfdDescriptor::isPairing() const
{
	return !isNull() && d->mDiscovery.isPairing();
}


const QList<QUrl>& IfdDescriptor::getAddresses() const
{
	static const QList<QUrl> EMPTY_ADDRESSES;

	return isNull() ? EMPTY_ADDRESSES : d->mDiscovery.getAddresses();
}


bool IfdDescriptor::isNull() const
{
	return d.data() == nullptr;
}


bool IfdDescriptor::isLocalIfd() const
{
	return !isNull() && d->mIsLocalIfd;
}


bool IfdDescriptor::isSameIfd(const IfdDescriptor& pOther) const
{
	return this == &pOther ||
		   (isNull() && pOther.isNull()) ||
		   (!isNull() && !pOther.isNull() && d->isSameIfd(*(pOther.d)));
}
