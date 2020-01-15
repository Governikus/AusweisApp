/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */


#include "Discovery.h"

#include "Initializer.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(MSG_TYPE, "msg")
VALUE_NAME(IFD_NAME, "IFDName")
VALUE_NAME(IFD_ID, "IFDID")
VALUE_NAME(PORT, "port")
VALUE_NAME(SUPPORTED_API, "SupportedAPI")
} // namespace


static Initializer::Entry E([] {
			qRegisterMetaType<QSharedPointer<const Discovery> >("QSharedPointer<const Discovery>");
		});


Discovery::Discovery(const QString& pIfdName, const QString& pIfdId, quint16 pPort, const QVector<IfdVersion::Version>& pSupportedApis)
	: RemoteMessage(RemoteCardMessageType::UNDEFINED)
	, mIfdName(pIfdName)
	, mIfdId(pIfdId)
	, mPort(pPort)
	, mSupportedApis(pSupportedApis)
{
}


Discovery::Discovery(const QJsonObject& pMessageObject)
	: RemoteMessage(RemoteCardMessageType::UNDEFINED)
	, mIfdName()
	, mIfdId()
	, mPort()
	, mSupportedApis()
{
	if (getStringValue(pMessageObject, MSG_TYPE()) != QLatin1String("REMOTE_IFD"))
	{
		markIncomplete(QStringLiteral("The value of msg should be REMOTE_IFD"));
	}

	mIfdName = getStringValue(pMessageObject, IFD_NAME());
	mIfdId = getStringValue(pMessageObject, IFD_ID());
	mPort = static_cast<quint16>(getIntValue(pMessageObject, PORT()));

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
	for (const auto& entry : array)
	{
		if (entry.isString())
		{
			mSupportedApis += IfdVersion(entry.toString()).getVersion();
			continue;
		}

		invalidType(SUPPORTED_API(), QLatin1String("string array"));
	}
}


Discovery::~Discovery()
{
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


QByteArray Discovery::toByteArray(const QString&) const
{
	QJsonObject result;

	result[IFD_NAME()] = mIfdName;
	result[IFD_ID()] = mIfdId;
	result[MSG_TYPE()] = QStringLiteral("REMOTE_IFD");
	result[PORT()] = mPort;

	QJsonArray levels;
	for (const auto& level : qAsConst(mSupportedApis))
	{
		levels += IfdVersion(level).toString();
	}
	result[SUPPORTED_API()] = levels;

	return RemoteMessage::toByteArray(result);
}
