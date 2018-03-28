/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDeviceDescriptor.h"

#include "Initializer.h"
#include "messages/Discovery.h"


using namespace governikus;


static Initializer::Entry E([] {
			qRegisterMetaType<RemoteDeviceDescriptor>("RemoteDeviceDescriptor");
		});


namespace
{
QHostAddress convertAddressProtocol(const QHostAddress& pAddress, QAbstractSocket::NetworkLayerProtocol pRequiredProtocol)
{
	const QAbstractSocket::NetworkLayerProtocol actualProtocol = pAddress.protocol();
	if (actualProtocol == pRequiredProtocol)
	{
		return pAddress;
	}

	switch (pRequiredProtocol)
	{
		case QAbstractSocket::IPv4Protocol:
		{
			bool conversionOK = false;
			const QHostAddress convertedAddress(pAddress.toIPv4Address(&conversionOK));
			return conversionOK ? convertedAddress : QHostAddress();
		}

		case QAbstractSocket::IPv6Protocol:
			return QHostAddress(pAddress.toIPv6Address());

		default:
			return QHostAddress();
	}
}


QUrl urlFromMsgAndHost(const QSharedPointer<const Discovery>& pMsg,
		const QHostAddress& pHostAddress,
		QAbstractSocket::NetworkLayerProtocol pRequiredProtocol)
{
	if (pMsg.isNull())
	{
		return QUrl();
	}

	const QHostAddress convertedHostAddress = convertAddressProtocol(pHostAddress, pRequiredProtocol);
	if (convertedHostAddress.isNull())
	{
		return QUrl();
	}

	QUrl url;
	url.setScheme(QStringLiteral("wss"));
	url.setHost(convertedHostAddress.toString());
	url.setPort(pMsg->getPort());

	return url;
}


}


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


RemoteDeviceDescriptor::RemoteDeviceDescriptorData* RemoteDeviceDescriptor::createRemoteDeviceDescriptorData(const QSharedPointer<const Discovery>& pMsg,
		const QHostAddress& pHostAddress,
		QAbstractSocket::NetworkLayerProtocol pRequiredProtocol)
{
	const QUrl url = urlFromMsgAndHost(pMsg, pHostAddress, pRequiredProtocol);
	if (url.isEmpty())
	{
		return nullptr;
	}

	const QString& ifdName = pMsg->getIfdName();
	const QString& ifdId = pMsg->getIfdId();
	const QVector<IfdVersion::Version>& supportedApis = pMsg->getSupportedApis();

	return new RemoteDeviceDescriptorData(ifdName, ifdId, supportedApis, url);
}


RemoteDeviceDescriptor::RemoteDeviceDescriptor(const QSharedPointer<const Discovery>& pDiscovery,
		const QHostAddress& pHostAddress,
		QAbstractSocket::NetworkLayerProtocol pRequiredProtocol)
	: d(createRemoteDeviceDescriptorData(pDiscovery, pHostAddress, pRequiredProtocol))
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
	return IfdVersion::selectLatestSupported(getApiVersions()).isValid();
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
