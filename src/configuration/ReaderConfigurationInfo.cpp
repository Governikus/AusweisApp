/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderConfigurationInfo.h"

#include "FileProvider.h"

using namespace governikus;


ReaderConfigurationInfo::ReaderConfigurationInfo()
//: LABEL ALL_PLATFORMS
	: ReaderConfigurationInfo(tr("Unknown reader"))
{
}


ReaderConfigurationInfo::ReaderConfigurationInfo(const QString& pReaderName)
	: d(new InternalInfo(false, 0, 0, pReaderName, QString(), QString(), QStringLiteral("default_reader.png"), QStringLiteral("default_reader_mit_ausweis.png")))
{
}


ReaderConfigurationInfo::ReaderConfigurationInfo(uint pVendorId, uint pProductId,
		const QString& pName, const QString& pUrl, const QString& pPattern, const QString& pIcon, const QString& pIconWithNPA)
	: d(new InternalInfo(true, pVendorId, pProductId, pName, pUrl, pPattern, pIcon, pIconWithNPA))
{
}


ReaderConfigurationInfo::~ReaderConfigurationInfo()
{
}


bool ReaderConfigurationInfo::operator ==(const ReaderConfigurationInfo& pOther) const
{
	return *d == *pOther.d;
}


bool ReaderConfigurationInfo::isKnownReader() const
{
	return d->mKnown;
}


uint ReaderConfigurationInfo::getVendorId() const
{
	return d->mVendorId;
}


uint ReaderConfigurationInfo::getProductId() const
{
	return d->mProductId;
}


const QString& ReaderConfigurationInfo::getName() const
{
	return d->mName;
}


const QString& ReaderConfigurationInfo::getUrl() const
{
#ifndef QT_NO_DEBUG
	if (QCoreApplication::applicationName() == QLatin1String("Test_configuration_ReaderConfiguration"))
	{
		// Make the reader available on all platforms
		static const QString url = QStringLiteral("https://www.governikus.de/");
		return url;
	}
#endif

	return d->mUrl;
}


const QString& ReaderConfigurationInfo::getPattern() const
{
	return d->mPattern;
}


QSharedPointer<UpdatableFile> ReaderConfigurationInfo::getIcon() const
{
	return Env::getSingleton<FileProvider>()->getFile(QStringLiteral("reader"), d->mIcon, QStringLiteral(":/images/reader/default_reader.png"));
}


QSharedPointer<UpdatableFile> ReaderConfigurationInfo::getIconWithNPA() const
{
	return Env::getSingleton<FileProvider>()->getFile(QStringLiteral("reader"), d->mIconWithNPA, QStringLiteral(":/images/reader/default_reader_mit_ausweis.png"));
}
