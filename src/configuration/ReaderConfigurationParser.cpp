/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderConfigurationParser.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QVersionNumber>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card_drivers)

ReaderConfigurationParser::EntryParser::EntryParser(const QJsonValue& pJsonValue)
	: mJsonValue(pJsonValue)
{
}


ReaderConfigurationParser::EntryParser::~EntryParser() = default;


QString ReaderConfigurationParser::EntryParser::getDriverUrl(const QJsonObject& pObject) const
{
	const QJsonValue driversValue = pObject.value(QLatin1String("Drivers"));
	if (!driversValue.isArray())
	{
		qCWarning(card_drivers) << "The value of 'Drivers' must be an array";
		return QString();
	}

	const QJsonArray& driversArray = driversValue.toArray();
	for (const QJsonValueConstRef entry : driversArray)
	{
		const QJsonObject& obj = entry.toObject();
		if (obj.isEmpty())
		{
			qCWarning(card_drivers) << "Drivers entry must be a valid object";
			return QString();
		}

		const QJsonValue& platforms = obj.value(QLatin1String("Platforms"));
		if (!platforms.isArray())
		{
			qCWarning(card_drivers) << "Invalid or missing Platforms tag";
			return QString();
		}

		const QString& url = obj.value(QLatin1String("URL")).toString();
		if (url.isEmpty())
		{
			qCWarning(card_drivers) << "Invalid or missing URL tag";
			return QString();
		}

		if (matchPlatform(platforms.toArray()))
		{
			return url;
		}
	}

	// No URL for this platform found, but entry is syntactically correct:
	// return empty non-null string.
	return QLatin1String("");
}


bool ReaderConfigurationParser::EntryParser::matchPlatform(const QJsonArray& pPlatforms, const QOperatingSystemVersion& pCurrentVersion) const
{
	QString currentOS;
	switch (pCurrentVersion.type())
	{
		case QOperatingSystemVersion::Windows:
			currentOS = QStringLiteral("win");
			break;

		case QOperatingSystemVersion::MacOS:
			currentOS = QStringLiteral("mac");
			break;

		default:
			currentOS = QStringLiteral("unknown");
	}

	const auto& parseSystemVersion = [&pCurrentVersion](const QString& pVersion){
				if (pVersion.isEmpty())
				{
					return pCurrentVersion;
				}

				const auto& number = QVersionNumber::fromString(pVersion);
				const auto minor = number.segmentCount() > 1 ? number.minorVersion() : -1;
				const auto micro = number.segmentCount() > 2 ? number.microVersion() : -1;
				return QOperatingSystemVersion(pCurrentVersion.type(), number.majorVersion(), minor, micro);
			};

	for (const QJsonValueConstRef entry : pPlatforms)
	{
		const auto& obj = entry.toObject();
		if (obj.value(QLatin1String("os")).toString() == currentOS)
		{
			const auto& min = obj.value(QLatin1String("min")).toString();
			const auto& max = obj.value(QLatin1String("max")).toString();

			if (pCurrentVersion >= parseSystemVersion(min) && pCurrentVersion <= parseSystemVersion(max))
			{
				return true;
			}
		}
	}

	return false;
}


QSet<uint> ReaderConfigurationParser::EntryParser::getProductIds(const QJsonObject& object, bool* parseOk) const
{
	static const auto productIdsKey = QLatin1String("ProductIds");
	if (object.contains(productIdsKey) && object[productIdsKey].isArray())
	{
		QSet<uint> productIds;
		const QJsonArray productIdsArray = object[productIdsKey].toArray();
		for (const QJsonValueConstRef value : productIdsArray)
		{
			if (!value.isString())
			{
				*parseOk = false;
				return {};
			}
			const uint parsedUInt = value.toString().toUInt(parseOk, 16);
			if (!*parseOk)
			{
				return {};
			}
			productIds.insert(parsedUInt);
		}
		return productIds;
	}

	static const auto productIdKey = QLatin1String("ProductId");
	if (object.contains(productIdKey) && object.value(productIdKey).isString())
	{
		const uint parsedUInt = object.value(productIdKey).toString().toUInt(parseOk, 16);
		if (!*parseOk)
		{
			return {};
		}
		return {parsedUInt};
	}

	*parseOk = false;
	return {};
}


ReaderConfigurationInfo ReaderConfigurationParser::EntryParser::parse() const
{
	if (!mJsonValue.isObject())
	{
		return fail(QStringLiteral("Cannot parse Json value: object expected"));
	}

	const QJsonObject& object = mJsonValue.toObject();

	bool parseOk = false;
	const uint vendorId = object.value(QLatin1String("VendorId")).toString().toUInt(&parseOk, 16);
	if (!parseOk)
	{
		return fail(QStringLiteral("Invalid or missing vendor id"));
	}

	const QSet<uint> productIds = getProductIds(object, &parseOk);
	if (!parseOk || productIds.isEmpty())
	{
		return fail(QStringLiteral("Invalid or missing product ids"));
	}

	const QString& name = object.value(QLatin1String("Name")).toString();
	if (name.isEmpty())
	{
		return fail(QStringLiteral("Invalid or missing name"));
	}

	const QString& url = getDriverUrl(object);
	if (url.isNull())
	{
		return fail(QStringLiteral("Invalid driver URL entry"));
	}

	// If Pattern is missing or empty => use name (exact match).
	const QString& pattern = object.value(QLatin1String("Pattern")).toString();

	const QString& icon = object.value(QLatin1String("Icon")).toString();

	return ReaderConfigurationInfo(vendorId, productIds, name, url, pattern, icon);
}


ReaderConfigurationInfo ReaderConfigurationParser::EntryParser::fail(const QString& pLogMessage) const
{
	qCWarning(card_drivers) << pLogMessage;

	return ReaderConfigurationInfo();
}


QList<ReaderConfigurationInfo> ReaderConfigurationParser::parse(const QByteArray& pData)
{
	QJsonParseError error {};
	QJsonDocument doc = QJsonDocument::fromJson(pData, &error);
	if (error.error != QJsonParseError::NoError)
	{
		return fail(QStringLiteral("Json parsing failed. Error at offset %1: %2").arg(error.offset).arg(error.errorString()));
	}

	const QJsonObject& rootObject = doc.object();
	if (rootObject.isEmpty())
	{
		return fail(QStringLiteral("Expected object at top level"));
	}

	if (!rootObject.contains(QStringLiteral("SupportedDevices")))
	{
		return fail(QStringLiteral("Root object does not contain a property named 'SupportedDevices'"));
	}

	// Root object OK. Look at child list.
	const QJsonValue& devicesValue = rootObject.value(QStringLiteral("SupportedDevices"));
	if (!devicesValue.isArray())
	{
		return fail(QStringLiteral("The value of 'SupportedDevices' must be an array"));
	}

	const QJsonArray& devicesArray = devicesValue.toArray();
	QList<ReaderConfigurationInfo> infos;
	for (const QJsonValueConstRef entry : devicesArray)
	{
		auto info = EntryParser(entry).parse();
		if (info.isKnownReader())
		{
			if (hasUniqueId(info, infos))
			{
				infos += info;
			}
			else
			{
				const auto& json = QString::fromUtf8(QJsonDocument(entry.toObject()).toJson());
				return fail(QStringLiteral("Invalid reader configuration entry: '%1' duplicate reader information").arg(json));
			}
		}
		else
		{
			return fail(QStringLiteral("Invalid reader configuration entry: ") + QString::fromUtf8(QJsonDocument(entry.toObject()).toJson()));
		}
	}

	return infos;
}


QList<ReaderConfigurationInfo> ReaderConfigurationParser::fail(const QString& pLogMessage)
{
	qCWarning(card_drivers) << pLogMessage;
	return QList<ReaderConfigurationInfo>();
}


bool ReaderConfigurationParser::hasUniqueId(const ReaderConfigurationInfo& pInfo, const QList<ReaderConfigurationInfo>& pInfos)
{
	const uint vendorId = pInfo.getVendorId();
	const QSet<uint> productIds = pInfo.getProductIds();
	const QString& name = pInfo.getName();
	const QString& pattern = pInfo.getPattern();

	for (const ReaderConfigurationInfo& info : pInfos)
	{
		if (vendorId > 0 && !productIds.isEmpty() && vendorId == info.getVendorId() && productIds.intersects(info.getProductIds()))
		{
			return false;
		}

		if (name == info.getName())
		{
			return false;
		}

		if (!pattern.isEmpty() && pattern == info.getPattern())
		{
			return false;
		}
	}

	return true;
}
