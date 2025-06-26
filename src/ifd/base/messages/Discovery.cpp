/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */


#include "Discovery.h"

#include "Initializer.h"
#include "RemoteServiceSettings.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QNetworkInterface>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(MSG_TYPE, "msg")
VALUE_NAME(IFD_NAME, "IFDName")
VALUE_NAME(IFD_ID, "IFDID")
VALUE_NAME(PORT, "port")
VALUE_NAME(ADDRESSES, "addresses")
VALUE_NAME(SUPPORTED_API, "SupportedAPI")
VALUE_NAME(PAIRING, "pairing")
} // namespace


INIT_FUNCTION([] {
			qRegisterMetaType<QSharedPointer<const Discovery>>("QSharedPointer<const Discovery>");
		})


void Discovery::parseSupportedApi(const QJsonObject& pMessageObject)
{
	if (!pMessageObject.contains(SUPPORTED_API()))
	{
		missingValue(SUPPORTED_API());
		return;
	}

	const auto& value = pMessageObject.value(SUPPORTED_API());
	if (!value.isArray())
	{
		invalidType(SUPPORTED_API(), QLatin1String("array"));
		return;
	}

	const auto& array = value.toArray();
	if (array.isEmpty())
	{
		markIncomplete(QStringLiteral("At least one entry is required for \"%1\"").arg(SUPPORTED_API()));
		return;
	}
	for (const QJsonValueConstRef entry : array)
	{
		if (entry.isString())
		{
			mSupportedApis += IfdVersion(entry.toString()).getVersion();
			continue;
		}

		invalidType(SUPPORTED_API(), QLatin1String("string array"));
	}
}


void Discovery::parseIfdId(const QJsonObject& pMessageObject)
{
	const auto ifdId = getStringValue(pMessageObject, IFD_ID()).toLatin1();
	if (isIncomplete())
	{
		return;
	}

	if (ifdId.isEmpty())
	{
		markIncomplete(QStringLiteral("The value of IFDID should not be empty"));
		return;
	}

	const QSslCertificate ifdCertificate(ifdId);
	if (!ifdCertificate.isNull())
	{
		mIfdId = RemoteServiceSettings::generateFingerprint(ifdCertificate);
		return;
	}

	mIfdId = QByteArray::fromHex(ifdId);
}


void Discovery::parseAddresses(const QJsonObject& pMessageObject)
{
	if (!pMessageObject.contains(ADDRESSES()))
	{
		return;
	}

	const auto& value = pMessageObject.value(ADDRESSES());
	if (!value.isArray())
	{
		invalidType(ADDRESSES(), QLatin1String("array"));
		return;
	}

	const auto& array = value.toArray();
	if (array.isEmpty())
	{
		markIncomplete(QStringLiteral("At least one entry is required for \"%1\"").arg(ADDRESSES()));
		return;
	}
	for (const QJsonValueConstRef entry : array)
	{
		if (!entry.isString())
		{
			invalidType(ADDRESSES(), QLatin1String("string"));
			continue;
		}

		QUrl url(entry.toString(), QUrl::StrictMode);
		if (!url.isValid())
		{
			invalidType(ADDRESSES(), QLatin1String("url"));
			continue;
		}

		if (url.scheme() != QStringLiteral("wss"))
		{
			markIncomplete(QStringLiteral("Found \"%1\" entry with wrong scheme: %2").arg(ADDRESSES(), url.toString()));
			continue;
		}

		if (url.host().isEmpty())
		{
			markIncomplete(QStringLiteral("Found \"%1\" entry without host: %2").arg(ADDRESSES(), url.toString()));
			continue;
		}

		if (url.port() == -1)
		{
			markIncomplete(QStringLiteral("Found \"%1\" entry without port: %2").arg(ADDRESSES(), url.toString()));
			continue;
		}

		mAddresses << url;
	}
}


void Discovery::parsePairing(const QJsonObject& pMessageObject)
{
	QList<IfdVersion::Version> sorted(mSupportedApis);
	std::sort(sorted.rbegin(), sorted.rend());

	if (sorted.isEmpty() || sorted.first() < IfdVersion::Version::v2)
	{
		const auto& pairing = pMessageObject.value(PAIRING());
		if (pMessageObject.contains(PAIRING()) && !pairing.isBool())
		{
			invalidType(PAIRING(), QLatin1String("boolean"));
		}
		mPairing = pairing.toBool();
		return;
	}

	mPairing = getBoolValue(pMessageObject, PAIRING());
}


Discovery::Discovery(const QString& pIfdName, const QByteArray& pIfdId, quint16 pPort, const QList<IfdVersion::Version>& pSupportedApis, bool pPairing)
	: IfdMessage(IfdMessageType::UNDEFINED)
	, mIfdName(pIfdName.toHtmlEscaped())
	, mIfdId(pIfdId)
	, mPort(pPort)
	, mAddresses()
	, mSupportedApis(pSupportedApis)
	, mPairing(pPairing)
{
}


Discovery::Discovery(const QJsonObject& pMessageObject)
	: IfdMessage(IfdMessageType::UNDEFINED)
	, mIfdName()
	, mIfdId()
	, mPort()
	, mAddresses()
	, mSupportedApis()
	, mPairing()
{
	if (getStringValue(pMessageObject, MSG_TYPE()) != QLatin1String("REMOTE_IFD"))
	{
		markIncomplete(QStringLiteral("The value of msg should be REMOTE_IFD"));
	}

	parseSupportedApi(pMessageObject);
	mIfdName = getStringValue(pMessageObject, IFD_NAME()).toHtmlEscaped();
	parseIfdId(pMessageObject);
	mPort = static_cast<quint16>(getIntValue(pMessageObject, PORT(), 0));
	parseAddresses(pMessageObject);
	parsePairing(pMessageObject);
}


const QString& Discovery::getIfdName() const
{
	return mIfdName;
}


const QByteArray& Discovery::getIfdId() const
{
	return mIfdId;
}


quint16 Discovery::getPort() const
{
	return mPort;
}


const QList<IfdVersion::Version>& Discovery::getSupportedApis() const
{
	return mSupportedApis;
}


void Discovery::setPairing(bool pEnabled)
{
	mPairing = pEnabled;
}


bool Discovery::isPairing() const
{
	return mPairing;
}


void Discovery::setAddresses(const QList<QHostAddress>& pAddresses)
{
	if (isIncomplete())
	{
		return;
	}

	mAddresses.clear();
	for (const auto& address : pAddresses)
	{
		if (address.isNull())
		{
			continue;
		}

		QUrl url;
		url.setScheme(QStringLiteral("wss"));
		url.setHost(address.toString());
		url.setPort(mPort);
		mAddresses << url;
	}
}


const QList<QUrl>& Discovery::getAddresses() const
{
	return mAddresses;
}


QByteArray Discovery::toByteArray(IfdVersion::Version pIfdVersion, const QString&) const
{
	QJsonObject result;

	result[MSG_TYPE()] = QStringLiteral("REMOTE_IFD");
	result[IFD_NAME()] = mIfdName;
	result[IFD_ID()] = QString::fromLatin1(mIfdId.toHex());
	result[PORT()] = mPort;

	QJsonArray levels;
	for (const auto& level : std::as_const(mSupportedApis))
	{
		levels += IfdVersion(level).toString();
	}
	result[SUPPORTED_API()] = levels;

	QJsonArray addresses;
	for (const auto& address : std::as_const(mAddresses))
	{
		addresses << address.toString();
	}
	result[ADDRESSES()] = addresses;

	if (pIfdVersion >= IfdVersion::Version::v2)
	{
		result[PAIRING()] = mPairing;
	}

	return IfdMessage::toByteArray(result);
}
