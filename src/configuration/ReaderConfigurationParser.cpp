/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
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


ReaderConfigurationParser::EntryParser::~EntryParser()
{
}


QString ReaderConfigurationParser::EntryParser::getDriverUrl(const QJsonObject& pObject) const
{
	const QJsonValue driversValue = pObject.value(QLatin1String("Drivers"));
	if (!driversValue.isArray())
	{
		qCWarning(card_drivers) << "The value of 'Drivers' must be an array";
		return QString();
	}

	const QJsonArray& driversArray = driversValue.toArray();
	for (const auto& entry : driversArray)
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

	for (const auto& entry : pPlatforms)
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

	const uint productId = object.value(QLatin1String("ProductId")).toString().toUInt(&parseOk, 16);
	if (!parseOk)
	{
		return fail(QStringLiteral("Invalid or missing product id"));
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
	const QString& iconWithNPA = object.value(QLatin1String("IconWithNPA")).toString();

	return ReaderConfigurationInfo(vendorId, productId, name, url, pattern, icon, iconWithNPA);
}


ReaderConfigurationInfo ReaderConfigurationParser::EntryParser::fail(const QString& pLogMessage) const
{
	qCWarning(card_drivers) << pLogMessage;

	return ReaderConfigurationInfo();
}


QVector<ReaderConfigurationInfo> ReaderConfigurationParser::parse(const QByteArray& pData)
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
	QVector<ReaderConfigurationInfo> infos;
	for (const auto& entry : devicesArray)
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


QVector<ReaderConfigurationInfo> ReaderConfigurationParser::fail(const QString& pLogMessage)
{
	qCWarning(card_drivers) << pLogMessage;
	return QVector<ReaderConfigurationInfo>();
}


bool ReaderConfigurationParser::hasUniqueId(const ReaderConfigurationInfo& pInfo, const QVector<ReaderConfigurationInfo>& pInfos)
{
	const uint vendorId = pInfo.getVendorId();
	const uint productId = pInfo.getProductId();
	const QString& name = pInfo.getName();
	const QString& pattern = pInfo.getPattern();

	for (const ReaderConfigurationInfo& info : pInfos)
	{
		if (vendorId > 0 && productId > 0 && vendorId == info.getVendorId() && productId == info.getProductId())
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
