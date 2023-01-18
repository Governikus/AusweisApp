/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "IfdDescriptor.h"

#include "Initializer.h"

using namespace governikus;

INIT_FUNCTION([] {
			qRegisterMetaType<IfdDescriptor>("IfdDescriptor");
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


IfdDescriptor::IfdDescriptorData::IfdDescriptorData(const QString& pIfdName,
		const QString& pIfdId,
		const QVector<IfdVersion::Version>& pApiVersions,
		const QUrl& pRemoteUrl,
		bool pIsLocalIfd)
	: mIfdName(pIfdName)
	, mIfdId(pIfdId)
	, mApiVersions(pApiVersions)
	, mUrl(pRemoteUrl)
	, mIsLocalIfd(pIsLocalIfd)
{
}


IfdDescriptor::IfdDescriptorData::~IfdDescriptorData() = default;


bool IfdDescriptor::IfdDescriptorData::operator==(const IfdDescriptorData& pOther) const
{
	return mIfdName == pOther.mIfdName &&
		   mIfdId == pOther.mIfdId &&
		   mApiVersions == pOther.mApiVersions &&
		   mUrl == pOther.mUrl;
}


bool IfdDescriptor::IfdDescriptorData::isSameIfd(const IfdDescriptorData& pOther) const
{
	return mIfdId == pOther.mIfdId;
}


IfdDescriptor::IfdDescriptor(const Discovery& pDiscovery, const QHostAddress& pHostAddress, bool pLocalIfd)
	: d()
{
	const QUrl url = urlFromMsgAndHost(pDiscovery, pHostAddress);
	if (url.isEmpty() || url.host().isEmpty())
	{
		return;
	}

	const QString& ifdName = pDiscovery.getIfdName();
	const QString& ifdId = pDiscovery.getIfdId();
	const QVector<IfdVersion::Version>& supportedApis = pDiscovery.getSupportedApis();
	d = new IfdDescriptorData(ifdName, ifdId, supportedApis, url, pLocalIfd);
}


const QString& IfdDescriptor::getIfdName() const
{
	static const QString EMPTY_STRING;

	return d.data() == nullptr ? EMPTY_STRING : d->mIfdName;
}


const QString& IfdDescriptor::getIfdId() const
{
	static const QString EMPTY_STRING;

	return d.data() == nullptr ? EMPTY_STRING : d->mIfdId;
}


const QVector<IfdVersion::Version>& IfdDescriptor::getApiVersions() const
{
	static const QVector<IfdVersion::Version> EMPTY_VECTOR;

	return d.data() == nullptr ? EMPTY_VECTOR : d->mApiVersions;
}


bool IfdDescriptor::isSupported() const
{
	return IfdVersion(IfdVersion::selectLatestSupported(getApiVersions())).isValid();
}


const QUrl& IfdDescriptor::getUrl() const
{
	static const QUrl EMPTY_URL;

	return d.data() == nullptr ? EMPTY_URL : d->mUrl;
}


bool IfdDescriptor::isNull() const
{
	return d.data() == nullptr;
}


bool IfdDescriptor::isLocalIfd() const
{
	return !isNull() && d->mIsLocalIfd;
}


bool IfdDescriptor::operator==(const IfdDescriptor& pOther) const
{
	return this == &pOther ||
		   (d.data() == nullptr && pOther.d.data() == nullptr) ||
		   (d.data() != nullptr && pOther.d.data() != nullptr && *d == *(pOther.d));
}


bool IfdDescriptor::isSameIfd(const IfdDescriptor& pOther) const
{
	return this == &pOther ||
		   (d.data() == nullptr && pOther.d.data() == nullptr) ||
		   (d.data() != nullptr && pOther.d.data() != nullptr && d->isSameIfd(*(pOther.d)));
}
