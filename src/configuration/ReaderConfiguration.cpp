/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderConfiguration.h"

#include "FileProvider.h"
#include "FuncUtils.h"
#include "ReaderConfigurationParser.h"

#include <QFile>
#include <QLoggingCategory>

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
		for (const ReaderConfigurationInfo& info : std::as_const(mReaderConfigurationInfos))
		{
			info.getIcon()->markDirty();
		}

		Q_EMIT fireUpdated();
	}
}


ReaderConfiguration::ReaderConfiguration()
	: mUpdatableFile(Env::getSingleton<FileProvider>()->getFile(QString(), QStringLiteral("supported-readers.json")))
	, mReaderConfigurationInfos()
{
	connect(mUpdatableFile.data(), &UpdatableFile::fireUpdated, this, &ReaderConfiguration::onFileUpdated);
	connect(mUpdatableFile.data(), &UpdatableFile::fireNoUpdateAvailable, this, &ReaderConfiguration::fireNoUpdateAvailable);
	mUpdatableFile->forEachLookupPath([this](const QString& pPath){return parseReaderConfiguration(pPath);});
}


void ReaderConfiguration::update()
{
	mUpdatableFile->update();
}


const QList<ReaderConfigurationInfo>& ReaderConfiguration::getReaderConfigurationInfos() const
{
	return mReaderConfigurationInfos;
}


QList<ReaderConfigurationInfo> ReaderConfiguration::getSupportedReaderConfigurationInfos() const
{
	return filter<ReaderConfigurationInfo>([](const ReaderConfigurationInfo& i){
			return !i.getUrl().isEmpty();
		}, std::as_const(mReaderConfigurationInfos));
}


QList<ReaderConfigurationInfo> ReaderConfiguration::getVirtualReaderConfigurationInfos() const
{
	return filter<ReaderConfigurationInfo>([](const ReaderConfigurationInfo& i){
			return i.getVendorId() == 0x0;
		}, std::as_const(mReaderConfigurationInfos));
}


ReaderConfigurationInfo ReaderConfiguration::getReaderConfigurationInfoById(const UsbId& pId) const
{
	for (const auto& info : std::as_const(mReaderConfigurationInfos))
	{
		if (pId.getVendorId() == info.getVendorId() && info.getProductIds().contains(pId.getProductId()))
		{
			return info;
		}
	}

	return ReaderConfigurationInfo();
}
