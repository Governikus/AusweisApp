/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDeviceDescriptor.h"

#include "Initializer.h"

using namespace governikus;

INIT_FUNCTION([] {
			qRegisterMetaType<RemoteDeviceDescriptor>("RemoteDeviceDescriptor");
		})

namespace
{

QUrl urlFromMsgAndHost(const Discovery& pDiscovery,
		const QHostAddress& pHostAddress)
{
	if (pDiscovery.isIncomplete())
	{
		return QUrl();
	}

	QUrl url;
	url.setScheme(QStringLiteral("wss"));
	url.setHost(pHostAddress.toString());
	url.setPort(pDiscovery.getPort());

	return url;
}


} // namespace


RemoteDeviceDescriptor::RemoteDeviceDescriptorData::RemoteDeviceDescriptorData(const QString& pIfdName,
		const QString& pIfdId,
		const QVector<IfdVersion::Version>& pApiVersions,
		const QUrl& pRemoteUrl)
	: mIfdName(pIfdName)
	, mIfdId(pIfdId)
	, mApiVersions(pApiVersions)
	, mUrl(pRemoteUrl)
{
}


RemoteDeviceDescriptor::RemoteDeviceDescriptorData::~RemoteDeviceDescriptorData()
{
}


bool RemoteDeviceDescriptor::RemoteDeviceDescriptorData::operator==(const RemoteDeviceDescriptorData& pOther) const
{
	return mIfdName == pOther.mIfdName &&
		   mIfdId == pOther.mIfdId &&
		   mApiVersions == pOther.mApiVersions &&
		   mUrl == pOther.mUrl;
}


bool RemoteDeviceDescriptor::RemoteDeviceDescriptorData::isSameIfd(const RemoteDeviceDescriptorData& pOther) const
{
	return mIfdId == pOther.mIfdId;
}


RemoteDeviceDescriptor::RemoteDeviceDescriptorData* RemoteDeviceDescriptor::createRemoteDeviceDescriptorData(const Discovery& pDiscovery,
		const QHostAddress& pHostAddress)
{
	const QUrl url = urlFromMsgAndHost(pDiscovery, pHostAddress);
	if (url.isEmpty() || url.host().isEmpty())
	{
		return nullptr;
	}

	const QString& ifdName = pDiscovery.getIfdName();
	const QString& ifdId = pDiscovery.getIfdId();
	const QVector<IfdVersion::Version>& supportedApis = pDiscovery.getSupportedApis();

	return new RemoteDeviceDescriptorData(ifdName, ifdId, supportedApis, url);
}


RemoteDeviceDescriptor::RemoteDeviceDescriptor(const Discovery& pDiscovery, const QHostAddress& pHostAddress)
	: d(createRemoteDeviceDescriptorData(pDiscovery, pHostAddress))
{
}


const QString& RemoteDeviceDescriptor::getIfdName() const
{
	static const QString EMPTY_STRING;

	return d.data() == nullptr ? EMPTY_STRING : d->mIfdName;
}


const QString& RemoteDeviceDescriptor::getIfdId() const
{
	static const QString EMPTY_STRING;

	return d.data() == nullptr ? EMPTY_STRING : d->mIfdId;
}


const QVector<IfdVersion::Version>& RemoteDeviceDescriptor::getApiVersions() const
{
	static const QVector<IfdVersion::Version> EMPTY_VECTOR;

	return d.data() == nullptr ? EMPTY_VECTOR : d->mApiVersions;
}


bool RemoteDeviceDescriptor::isSupported() const
{
	return IfdVersion(IfdVersion::selectLatestSupported(getApiVersions())).isValid();
}


const QUrl& RemoteDeviceDescriptor::getUrl() const
{
	static const QUrl EMPTY_URL;

	return d.data() == nullptr ? EMPTY_URL : d->mUrl;
}


bool RemoteDeviceDescriptor::isNull() const
{
	return d.data() == nullptr;
}


bool RemoteDeviceDescriptor::operator==(const RemoteDeviceDescriptor& pOther) const
{
	return this == &pOther ||
		   (d.data() == nullptr && pOther.d.data() == nullptr) ||
		   (d.data() != nullptr && pOther.d.data() != nullptr && *d == *(pOther.d));
}


bool RemoteDeviceDescriptor::isSameIfd(const RemoteDeviceDescriptor& pOther) const
{
	return this == &pOther ||
		   (d.data() == nullptr && pOther.d.data() == nullptr) ||
		   (d.data() != nullptr && pOther.d.data() != nullptr && d->isSameIfd(*(pOther.d)));
}
