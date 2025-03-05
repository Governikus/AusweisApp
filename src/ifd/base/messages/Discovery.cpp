/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */


#include "Discovery.h"

#include "Initializer.h"
#include "RemoteServiceSettings.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(MSG_TYPE, "msg")
VALUE_NAME(IFD_NAME, "IFDName")
VALUE_NAME(IFD_ID, "IFDID")
VALUE_NAME(PORT, "port")
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
	, mSupportedApis(pSupportedApis)
	, mPairing(pPairing)
{
}


Discovery::Discovery(const QJsonObject& pMessageObject)
	: IfdMessage(IfdMessageType::UNDEFINED)
	, mIfdName()
	, mIfdId()
	, mPort()
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


bool Discovery::getPairing() const
{
	return mPairing;
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

	if (pIfdVersion >= IfdVersion::Version::v2)
	{
		result[PAIRING()] = mPairing;
	}

	return IfdMessage::toByteArray(result);
}
