/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */


#include "Discovery.h"

#include "Initializer.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


static Initializer::Entry E([] {
			qRegisterMetaType<QSharedPointer<const Discovery> >("QSharedPointer<const Discovery>");
		});


Discovery::Discovery(const QString& pIfdName, const QString& pIfdId, quint16 pPort, const QVector<IfdVersion::Version>& pSupportedApis)
	: mIfdName(pIfdName)
	, mIfdId(pIfdId)
	, mPort(pPort)
	, mSupportedApis(pSupportedApis)
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


QJsonDocument Discovery::toJson() const
{
	QJsonObject result;

	result[QLatin1String("IFDName")] = mIfdName;
	result[QLatin1String("IFDID")] = mIfdId;
	result[QLatin1String("msg")] = QStringLiteral("REMOTE_IFD");
	result[QLatin1String("port")] = mPort;

	QJsonArray levels;
	for (const auto& level : qAsConst(mSupportedApis))
	{
		levels += IfdVersion(level).toString();
	}
	result[QLatin1String("SupportedAPI")] = levels;

	return QJsonDocument(result);
}
