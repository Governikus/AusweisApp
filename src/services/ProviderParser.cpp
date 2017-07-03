/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "LanguageString.h"
#include "ProviderParser.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


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
		return QSharedPointer<ProviderSettings>();
	}
	QJsonObject doc = json.object();

	QSharedPointer<ProviderSettings> providerSettings(new ProviderSettings());
	providerSettings->setIssueDate(doc["issueDate"].toVariant().toDateTime());

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
				prov["clientUrl"].toString());
	}
	providerSettings->setProviders(providers);

	return providerSettings;
}
