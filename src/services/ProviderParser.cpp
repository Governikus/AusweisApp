/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "LanguageString.h"
#include "ProviderParser.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(update)


using namespace governikus;


ProviderParser::ProviderParser()
	: SettingsParser<ProviderSettings>()
{
}


ProviderParser::~ProviderParser()
{
}


QSharedPointer<ProviderSettings> ProviderParser::parse(const QByteArray& pData)
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

	QJsonArray array = doc["provider"].toArray();
	QVector<Provider> providers(array.size());
	for (int i = 0; i < array.size(); ++i)
	{
		const QJsonObject prov = array[i].toObject();
		const QString postalAddressCamel = prov["postalAddress"].toString();
		const QString postalAddressLower = prov["postaladdress"].toString();
		providers[i] = Provider(
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
