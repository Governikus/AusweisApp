/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderConfiguration.h"

#include "Env.h"
#include "FileDestination.h"
#include "FileProvider.h"
#include "FuncUtils.h"
#include "ReaderConfigurationParser.h"
#include "SingletonHelper.h"

#include <QFile>
#include <QLoggingCategory>
#include <QRegularExpression>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(configuration)


defineSingleton(ReaderConfiguration)


bool ReaderConfiguration::parseReaderConfiguration()
{
	const QString& path = mUpdatableFile->lookupPath();
	if (!QFile::exists(path))
	{
		qCCritical(configuration) << "ReaderConfiguration file not found";
		return false;
	}

	QFile configFile(path);
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qCCritical(configuration) << "Wasn't able to open ReaderConfiguration file";
		return false;
	}

	const auto& readerConfigurationInfos = ReaderConfigurationParser::parse(configFile.readAll());
	if (readerConfigurationInfos.isEmpty())
	{
		qCCritical(configuration) << "Parse error while reading ReaderConfiguration";
		return false;
	}

	mReaderConfigurationInfos = readerConfigurationInfos;
	return true;
}


void ReaderConfiguration::onFileUpdated()
{
	if (parseReaderConfiguration())
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
	parseReaderConfiguration();
}


ReaderConfiguration& ReaderConfiguration::getInstance()
{
	return *Instance;
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


const ReaderConfigurationInfo ReaderConfiguration::getRemoteReaderConfigurationInfo() const
{
	return getReaderConfigurationInfoById(UsbId());
}


const QVector<ReaderConfigurationInfo>& ReaderConfiguration::getReaderConfigurationInfos() const
{
	return mReaderConfigurationInfos;
}


const QVector<ReaderConfigurationInfo> ReaderConfiguration::getSupportedReaderConfigurationInfos() const
{
	return filter<ReaderConfigurationInfo>([](const ReaderConfigurationInfo& i){
				return !i.getUrl().isEmpty();
			}, qAsConst(mReaderConfigurationInfos));
}


#ifndef QT_NO_DEBUG
ReaderConfigurationInfo ReaderConfiguration::getReaderConfigurationInfo(const QString& pReaderName) const
{
	for (const auto& info : qAsConst(mReaderConfigurationInfos))
	{
		const QString& pattern = info.getPattern();
		const QRegularExpression expression(pattern.isEmpty() ? info.getName() : pattern);
		if (pReaderName.contains(expression))
		{
			return info;
		}
	}

	return ReaderConfigurationInfo(pReaderName);
}


#endif


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
