/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderConfigurationParser.h"

#include "LanguageString.h"

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


} // namespace

bool ProviderConfigurationParser::isExcludedPlatform(const QJsonArray& pExcludedArray, QLatin1String pCurrentOS)
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


QVector<ProviderConfigurationInfo> ProviderConfigurationParser::parseProvider(const QByteArray& pData, QLatin1String pCurrentOS)
{
	QJsonParseError jsonError;
	const auto& json = QJsonDocument::fromJson(pData, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		qCCritical(update) << "Cannot parse providers:" << jsonError.errorString();
		return QVector<ProviderConfigurationInfo>();
	}
	QJsonObject doc = json.object();

	const QJsonArray& array = doc[QLatin1String("provider")].toArray();
	QVector<ProviderConfigurationInfo> providers;
	providers.reserve(array.size());
	for (const auto& entry : array)
	{
		const QJsonObject prov = entry.toObject();

		if (isExcludedPlatform(prov[QLatin1String("exclude")].toArray(), pCurrentOS))
		{
			continue;
		}

		const QString postalAddressCamel = prov[QLatin1String("postalAddress")].toString();
		const QString postalAddressLower = prov[QLatin1String("postaladdress")].toString();
		providers << ProviderConfigurationInfo(
				LanguageString(prov[QLatin1String("shortName")]),
				LanguageString(prov[QLatin1String("longName")]),
				LanguageString(prov[QLatin1String("shortDescription")]),
				LanguageString(prov[QLatin1String("longDescription")]),
				prov[QLatin1String("address")].toString(),
				prov[QLatin1String("homepage")].toString(),
				prov[QLatin1String("category")].toString(),
				prov[QLatin1String("phone")].toString(),
				prov[QLatin1String("email")].toString(),
				postalAddressCamel.isEmpty() ? postalAddressLower : postalAddressCamel, // Accept old format
				prov[QLatin1String("icon")].toString(),
				prov[QLatin1String("image")].toString(),
				prov[QLatin1String("subjectUrls")].toVariant().toStringList(),
				prov[QLatin1String("subjectUrlInfo")].toString());
	}

	return providers;
}


QMap<QString, CallCost> ProviderConfigurationParser::parseCallCosts(const QByteArray& pData)
{
	QJsonParseError jsonError;
	const auto& json = QJsonDocument::fromJson(pData, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		qCCritical(update) << "Cannot parse call costs:" << jsonError.errorString();
		return QMap<QString, CallCost>();
	}
	QJsonObject doc = json.object();

	QMap<QString, CallCost> callCosts;
	const auto& callCostArray = doc[QLatin1String("callcosts")].toArray();
	for (const auto& callCostElem : callCostArray)
	{
		const auto cost = CallCost(callCostElem);
		const auto& prefixArray = callCostElem.toObject()[QLatin1String("prefixes")].toArray();
		for (const auto& prefixElem : prefixArray)
		{
			const auto& prefix = prefixElem.toString();
			callCosts.insert(prefix, cost);
		}
	}

	return callCosts;
}


QVector<ProviderConfigurationInfo> ProviderConfigurationParser::parseProvider(const QByteArray& pData)
{
	return parseProvider(pData, getCurrentOS());
}
