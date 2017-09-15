/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "LanguageString.h"
#include "ProviderParser.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(update)


using namespace governikus;

namespace
{

inline QLatin1String getCurrentOS()
{
#if defined(Q_OS_WIN)
	return QLatin1String("win");

#elif defined(Q_OS_MACOS)
	return QLatin1String("mac");

#elif defined(Q_OS_IOS)
	return QLatin1String("ios");

#elif defined(Q_OS_ANDROID)
	return QLatin1String("android");

#elif defined(Q_OS_LINUX)
	return QLatin1String("linux");

#elif defined(Q_OS_BSD4)
	return QLatin1String("bsd");

#else
#error OS not implemented
#endif
}


}

ProviderParser::ProviderParser()
	: SettingsParser<ProviderSettings>()
{
}


ProviderParser::~ProviderParser()
{
}


QSharedPointer<ProviderSettings> ProviderParser::parse(const QByteArray& pData)
{
	return parse(pData, getCurrentOS());
}


QSharedPointer<ProviderSettings> ProviderParser::parse(const QByteArray& pData, QLatin1String pCurrentOS)
{
	QJsonParseError jsonError;
	const auto& json = QJsonDocument::fromJson(pData, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		qCCritical(update) << "Cannot parse provider settings:" << jsonError.errorString();
		return QSharedPointer<ProviderSettings>();
	}
	QJsonObject doc = json.object();

	QSharedPointer<ProviderSettings> providerSettings(new ProviderSettings());
	providerSettings->setIssueDate(doc["issueDate"].toVariant().toDateTime());

	QMap<QString, CallCost> callCosts;
	const auto& callCostArray = doc["callcosts"].toArray();
	for (const auto& callCostElem : callCostArray)
	{
		const auto cost = CallCost(callCostElem);
		const auto& prefixArray = callCostElem.toObject()["prefixes"].toArray();
		for (const auto& prefixElem : prefixArray)
		{
			const auto& prefix = prefixElem.toString();
			callCosts.insert(prefix, cost);
		}
	}
	providerSettings->setCallCosts(callCosts);

	const QJsonArray& array = doc["provider"].toArray();
	QVector<Provider> providers;
	providers.reserve(array.size());
	for (const auto& entry : array)
	{
		const QJsonObject prov = entry.toObject();

		if (isExcludedPlatform(prov["exclude"].toArray(), pCurrentOS))
		{
			continue;
		}

		const QString postalAddressCamel = prov["postalAddress"].toString();
		const QString postalAddressLower = prov["postaladdress"].toString();
		providers << Provider(
				LanguageString(prov["shortName"]),
				LanguageString(prov["longName"]),
				LanguageString(prov["shortDescription"]),
				LanguageString(prov["longDescription"]),
				prov["address"].toString(),
				prov["homepage"].toString(),
				prov["category"].toString(),
				prov["phone"].toString(),
				prov["email"].toString(),
				postalAddressCamel.isEmpty() ? postalAddressLower : postalAddressCamel, // Accept old format
				prov["icon"].toString(),
				prov["image"].toString(),
				prov["tcTokenUrl"].toString(),
				prov["clientUrl"].toString(),
				prov["subjectUrls"].toVariant().toStringList());
	}
	providerSettings->setProviders(providers);

	return providerSettings;
}


bool ProviderParser::isExcludedPlatform(const QJsonArray& pExcludedArray, QLatin1String pCurrentOS)
{
	const QLatin1String osType = pCurrentOS == QLatin1String("ios") || pCurrentOS == QLatin1String("android")
			? QLatin1String("mobile")
			: QLatin1String("desktop");

	for (const auto& entry : pExcludedArray)
	{
		const QString& value = entry.toString().toLower();
		if (value == pCurrentOS || value == osType)
		{
			return true;
		}
	}
	return false;
}
