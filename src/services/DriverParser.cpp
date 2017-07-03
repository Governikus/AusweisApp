/*!
 * DriverParser.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "DriverParser.h"
#include "SmartCardDefinitions.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QStringList>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card_drivers)


namespace
{
const QStringList SUPPORTED_READERS(
	{
		getEnumName(ReaderType::REINER_cyberJack_RFID_komfort),
		getEnumName(ReaderType::REINER_cyberJack_RFID_standard),
		getEnumName(ReaderType::REINER_cyberJack_RFID_basis),
		getEnumName(ReaderType::SCM_SDI011),
		getEnumName(ReaderType::SCM_SCL011_Contactless_Reader)
	});


}


/**
 * Parse a single entry of a driver list.
 */
class EntryParser
{
	private:
		const QJsonValue& mJsonValue;

		const QMap<QString, QSysInfo::WinVersion> mWinVersion;

		const QMap<QString, QSysInfo::MacVersion> mMacVersion;

		QString getReaderType(const QJsonObject& pObject) const;

		QString getDriverUrl(const QJsonObject& pObject) const;

		bool matchPlatform(const QJsonArray& pPlatforms) const;

		QSharedPointer<Driver> fail(const QString& logMessage) const;

	public:
		EntryParser(const QJsonValue& pJsonValue);

		virtual ~EntryParser();

		QSharedPointer<const Driver> parse() const;
};


EntryParser::EntryParser(const QJsonValue& pJsonValue)
	: mJsonValue(pJsonValue)
	, mWinVersion(
		{
			{"WV_WINDOWS7", QSysInfo::WV_WINDOWS7},
			{"WV_WINDOWS8", QSysInfo::WV_WINDOWS8},
			{"WV_WINDOWS8_1", QSysInfo::WV_WINDOWS8_1},
			{"WV_WINDOWS10", QSysInfo::WV_WINDOWS10}
		}


		)
	, mMacVersion({
				{"MV_10_9", QSysInfo::MV_10_9},
				{"MV_10_10", QSysInfo::MV_10_10},
				{"MV_10_11", QSysInfo::MV_10_11}
			}
			)
{
}


EntryParser::~EntryParser()
{
}


QString EntryParser::getReaderType(const QJsonObject& pObject) const
{
	const QString val = pObject.value(QStringLiteral("ReaderType")).toString();
	return SUPPORTED_READERS.contains(val) ? val : QString();
}


QString EntryParser::getDriverUrl(const QJsonObject& pObject) const
{
	static const QString DRIVERS("Drivers");
	static const QString PLATFORMS("Platforms");
	static const QString URL("URL");

	const QJsonValue driversValue = pObject.value(DRIVERS);
	if (!driversValue.isArray())
	{
		qCWarning(card_drivers) << "The value of 'Drivers' must be an array";

		return QString();
	}

	const QJsonArray driversArray = driversValue.toArray();
	const int count = driversArray.count();
	for (int index = 0; index < count; index++)
	{
		const QJsonObject entry = driversArray.at(index).toObject();
		if (entry.isEmpty())
		{
			qCWarning(card_drivers) << "Drivers entry must be a valid object";

			return QString();
		}

		const QJsonValue platforms = entry.value(PLATFORMS);
		if (!platforms.isArray())
		{
			qCWarning(card_drivers) << "Invalid or missing Platforms tag";

			return QString();
		}

		const QString url = entry.value(URL).toString();
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
	return QStringLiteral("");
}


bool EntryParser::matchPlatform(const QJsonArray& pPlatforms) const
{
	const int count = pPlatforms.count();
	for (int index = 0; index < count; index++)
	{
		const QString platform = pPlatforms.at(index).toString();

#if defined(Q_OS_OSX)
		if (QSysInfo::MacintoshVersion == mMacVersion.value(platform.trimmed(), QSysInfo::MV_None))
		{
			return true;
		}
#elif defined(Q_OS_WIN)
		if (QSysInfo::WindowsVersion == mWinVersion.value(platform.trimmed(), QSysInfo::WV_None))
		{
			return true;
		}
#elif defined(Q_OS_LINUX)
		if (platform == QLatin1String("LINUX"))
		{
			return true;
		}
#else
		Q_UNUSED(platform)
#endif
	}

	return false;
}


QSharedPointer<const Driver> EntryParser::parse() const
{
	static const QString VENDOR_ID("VendorId");
	static const QString PRODUCT_ID("ProductId");
	static const QString NAME("Name");

	if (!mJsonValue.isObject())
	{
		return fail(QStringLiteral("Cannot parse Json value: object expected"));
	}

	const QJsonObject object = mJsonValue.toObject();
	const QString readerType = getReaderType(object);
	if (readerType.isEmpty())
	{
		return fail(QStringLiteral("Invalid or missing reader type"));
	}

	bool parseOk = false;
	const uint vendorId = object.value(VENDOR_ID).toString().toUInt(&parseOk, 16);
	if (!parseOk)
	{
		return fail(QStringLiteral("Invalid or missing vendor id"));
	}

	const uint productId = object.value(PRODUCT_ID).toString().toUInt(&parseOk, 16);
	if (!parseOk)
	{
		return fail(QStringLiteral("Invalid or missing product id"));
	}

	const QString name = object.value(NAME).toString();
	if (name.isEmpty())
	{
		return fail(QStringLiteral("Invalid or missing name"));
	}

	const QString url = getDriverUrl(object);
	if (url.isNull())
	{
		return fail(QStringLiteral("Invalid driver URL entry"));
	}

	return QSharedPointer<const Driver>(new Driver(readerType, vendorId, productId, name, url));
}


QSharedPointer<Driver> EntryParser::fail(const QString& pLogMessage) const
{
	qCWarning(card_drivers) << pLogMessage;

	return QSharedPointer<Driver>();
}


DriverParser::DriverParser()
	: SettingsParser<DriverSettings>()
{
}


DriverParser::~DriverParser()
{
}


QSharedPointer<DriverSettings> DriverParser::parse(const QByteArray& pData)
{
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(pData, &error);
	if (error.error != QJsonParseError::NoError)
	{
		return fail(QString("Json parsing failed. Error at offset %1: %2").arg(error.offset).arg(error.errorString()));
	}

	const QJsonObject rootObject = doc.object();
	if (rootObject.isEmpty())
	{
		return fail(QStringLiteral("Expected object at top level"));
	}

	if (!rootObject.contains(QStringLiteral("IssueDate")))
	{
		return fail(QStringLiteral("Root object does not contain a property named 'IssueDate'"));
	}

	const QJsonValue issueDateValue = rootObject.value(QStringLiteral("IssueDate"));
	if (!issueDateValue.isString())
	{
		return fail(QStringLiteral("The value of 'IssueDate' must be a string"));
	}

	const QString issueDateString = issueDateValue.toString();
	const QDateTime issueDate = QDateTime::fromString(issueDateString, Qt::ISODate);
	if (!issueDate.isValid())
	{
		return fail(QStringLiteral("Error parsing issue date"));
	}

	if (!rootObject.contains(QStringLiteral("SupportedDevices")))
	{
		return fail(QStringLiteral("Root object does not contain a property named 'SupportedDevices'"));
	}

	// Root object OK. Look at child list.
	const QJsonValue devicesValue = rootObject.value(QStringLiteral("SupportedDevices"));
	if (!devicesValue.isArray())
	{
		return fail(QStringLiteral("The value of 'SupportedDevices' must be an array"));
	}

	const QJsonArray devicesArray = devicesValue.toArray();
	const int count = devicesArray.count();
	QVector<QSharedPointer<const Driver> > drivers;
	for (int index = 0; index < count; index++)
	{
		const QJsonValue entry = devicesArray.at(index);
		auto driver = EntryParser(entry).parse();
		if (driver)
		{
			if (!driver->getUrl().isEmpty())
			{
				drivers += driver;
			}
		}
		else
		{
			return fail(QStringLiteral("Invalid driver entry: ") + entry.toString());
		}
	}

	return QSharedPointer<DriverSettings>(new DriverSettings(issueDate, drivers));
}


QSharedPointer<DriverSettings> DriverParser::fail(const QString& pLogMessage)
{
	qCWarning(card_drivers) << pLogMessage;

	return QSharedPointer<DriverSettings>();
}
