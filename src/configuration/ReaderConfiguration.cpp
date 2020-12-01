/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderConfiguration.h"

#include "FileProvider.h"
#include "FuncUtils.h"
#include "ReaderConfigurationParser.h"

#include <QFile>
#include <QLoggingCategory>
#include <QRegularExpression>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(configuration)

bool ReaderConfiguration::parseReaderConfiguration(const QString& pPath)
{
	if (pPath.isEmpty() || !QFile::exists(pPath))
	{
		qCCritical(configuration) << "ReaderConfiguration file not found:" << pPath;
		return false;
	}

	QFile configFile(pPath);
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qCCritical(configuration) << "Wasn't able to open ReaderConfiguration file:" << pPath;
		return false;
	}

	const auto& readerConfigurationInfos = ReaderConfigurationParser::parse(configFile.readAll());
	if (readerConfigurationInfos.isEmpty())
	{
		qCCritical(configuration) << "Parse error while reading ReaderConfiguration:" << pPath;
		return false;
	}

	mReaderConfigurationInfos = readerConfigurationInfos;
	return true;
}


void ReaderConfiguration::onFileUpdated()
{
	if (mUpdatableFile->forEachLookupPath([this](const QString& pPath){return parseReaderConfiguration(pPath);}))
	{
		for (const ReaderConfigurationInfo& info : qAsConst(mReaderConfigurationInfos))
		{
			info.getIcon()->markDirty();
			info.getIconWithNPA()->markDirty();
		}

		Q_EMIT fireUpdated();
	}
}


ReaderConfiguration::ReaderConfiguration()
	: mUpdatableFile(Env::getSingleton<FileProvider>()->getFile(QString(), QStringLiteral("supported-readers.json")))
	, mReaderConfigurationInfos()
{
	connect(mUpdatableFile.data(), &UpdatableFile::fireUpdated, this, &ReaderConfiguration::onFileUpdated);
	mUpdatableFile->forEachLookupPath([this](const QString& pPath){return parseReaderConfiguration(pPath);});
}


QString ReaderConfiguration::getNoReaderFoundIconPath()
{
	return QStringLiteral(":/images/reader/default_no_reader.png");
}


QString ReaderConfiguration::getMultipleReaderIconPath()
{
	return QStringLiteral(":/images/reader/default_more_reader.png");
}


void ReaderConfiguration::update()
{
	mUpdatableFile->update();
}


const QVector<ReaderConfigurationInfo>& ReaderConfiguration::getReaderConfigurationInfos() const
{
	return mReaderConfigurationInfos;
}


QVector<ReaderConfigurationInfo> ReaderConfiguration::getSupportedReaderConfigurationInfos() const
{
	return filter<ReaderConfigurationInfo>([](const ReaderConfigurationInfo& i){
				return !i.getUrl().isEmpty();
			}, qAsConst(mReaderConfigurationInfos));
}


QVector<ReaderConfigurationInfo> ReaderConfiguration::getVirtualReaderConfigurationInfos() const
{
	return filter<ReaderConfigurationInfo>([](const ReaderConfigurationInfo& i){
				return i.getVendorId() == 0x0;
			}, qAsConst(mReaderConfigurationInfos));
}


ReaderConfigurationInfo ReaderConfiguration::getReaderConfigurationInfoById(const UsbId& pId) const
{
	for (const auto& info : qAsConst(mReaderConfigurationInfos))
	{
		if (pId.getVendorId() == info.getVendorId() && pId.getProductId() == info.getProductId())
		{
			return info;
		}
	}

	return ReaderConfigurationInfo();
}
