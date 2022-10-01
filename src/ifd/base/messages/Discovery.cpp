/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
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

#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
	using JsonValueRef = const QJsonValueConstRef;
#elif (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	using JsonValueRef = const QJsonValueRef;
#else
	using JsonValueRef = const QJsonValue&;
#endif

	const auto& array = value.toArray();
	for (JsonValueRef entry : array)
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
	mIfdId = getStringValue(pMessageObject, IFD_ID());
	if (isIncomplete())
	{
		return;
	}

	QVector<IfdVersion::Version> sorted(mSupportedApis);
	std::sort(sorted.rbegin(), sorted.rend());
	const bool expectCertificate = !sorted.isEmpty() && sorted.first() >= IfdVersion::Version::v2;

	const bool isFingerprint = mIfdId.size() == 64;
	if (isFingerprint)
	{
		if (expectCertificate)
		{
			invalidType(IFD_ID(), QLatin1String("X.509 certificate (PEM)"));
		}
		return;
	}

	const QSslCertificate ifdCertificate(mIfdId.toLatin1());
	if (!ifdCertificate.isNull())
	{
		if (!expectCertificate)
		{
			invalidType(IFD_ID(), QLatin1String("certificate fingerprint (SHA256)"));
		}
		mIfdId = RemoteServiceSettings::generateFingerprint(ifdCertificate);
		return;
	}

	if (expectCertificate)
	{
		invalidType(IFD_ID(), QLatin1String("X.509 certificate (PEM)"));
	}
	else
	{
		invalidType(IFD_ID(), QLatin1String("certificate fingerprint (SHA256)"));
	}
}


void Discovery::parsePairing(const QJsonObject& pMessageObject)
{
	QVector<IfdVersion::Version> sorted(mSupportedApis);
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


Discovery::Discovery(const QString& pIfdName, const QString& pIfdId, quint16 pPort, const QVector<IfdVersion::Version>& pSupportedApis)
	: IfdMessage(IfdMessageType::UNDEFINED)
	, mIfdName(pIfdName)
	, mIfdId(pIfdId)
	, mPort(pPort)
	, mSupportedApis(pSupportedApis)
	, mPairing(false)
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
	mIfdName = getStringValue(pMessageObject, IFD_NAME());
	parseIfdId(pMessageObject);
	mPort = static_cast<quint16>(getIntValue(pMessageObject, PORT(), 0));
	parsePairing(pMessageObject);
}


const QString& Discovery::getIfdName() const
{
	return mIfdName;
}


const QString& Discovery::getIfdId() const
{
	return mIfdId;
}


quint16 Discovery::getPort() const
{
	return mPort;
}


const QVector<IfdVersion::Version>& Discovery::getSupportedApis() const
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
	result[IFD_ID()] = mIfdId;
	result[PORT()] = mPort;

	QJsonArray levels;
	for (const auto& level : qAsConst(mSupportedApis))
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
