/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderConfigurationInfo.h"

#include "FileProvider.h"


using namespace governikus;


QString ProviderConfigurationInfo::getDefaultFile(const QString& pSuffix) const
{
#ifdef Q_OS_IOS
	const QString platform = QStringLiteral("ios/");
#else
	const QString platform;
#endif
	const QString category = d->mParams.mCategory.isEmpty() ? QStringLiteral("general") : d->mParams.mCategory;
	return QStringLiteral(":/images/provider/%1%2%3.svg").arg(platform, category, pSuffix);
}


ProviderConfigurationInfo::ProviderConfigurationInfo(const ProviderConfigurationInfoParams& pParams)
	: d(new InternalInfo(pParams))
{
}


ProviderConfigurationInfo::~ProviderConfigurationInfo() = default;


bool ProviderConfigurationInfo::operator ==(const ProviderConfigurationInfo& pOther) const
{
	return *d == *pOther.d;
}


bool ProviderConfigurationInfo::operator !=(const ProviderConfigurationInfo& pOther) const
{
	return !(*d == *pOther.d);
}


const LanguageString& ProviderConfigurationInfo::getShortName() const
{
	return !d->mParams.mShortName.toString().isNull() ? d->mParams.mShortName : getLongName();
}


const LanguageString& ProviderConfigurationInfo::getLongName() const
{
	return d->mParams.mLongName;
}


const LanguageString& ProviderConfigurationInfo::getLongDescription() const
{
	return d->mParams.mLongDescription;
}


const QString& ProviderConfigurationInfo::getAddress() const
{
	return d->mParams.mAddress;
}


QString ProviderConfigurationInfo::getAddressDomain() const
{
	return QUrl::fromUserInput(d->mParams.mAddress).host();
}


const QString& ProviderConfigurationInfo::getHomepage() const
{
	return d->mParams.mHomepage;
}


QString ProviderConfigurationInfo::getHomepageBase() const
{
	return QUrl::fromUserInput(d->mParams.mHomepage).host();
}


const QString& ProviderConfigurationInfo::getCategory() const
{
	return d->mParams.mCategory;
}


const QString& ProviderConfigurationInfo::getPhone() const
{
	return d->mParams.mPhone;
}


const QString& ProviderConfigurationInfo::getEMail() const
{
	return d->mParams.mEmail;
}


const QString& ProviderConfigurationInfo::getPostalAddress() const
{
	return d->mParams.mPostalAddress;
}


QSharedPointer<UpdatableFile> ProviderConfigurationInfo::getIcon() const
{
	const QString defaultFile = getDefaultFile(QStringLiteral("_button"));
	return Env::getSingleton<FileProvider>()->getFile(QStringLiteral("provider"), d->mParams.mIcon, defaultFile);
}


QSharedPointer<UpdatableFile> ProviderConfigurationInfo::getImage() const
{
	const QString defaultFile = getDefaultFile(QStringLiteral("_bg"));
	return Env::getSingleton<FileProvider>()->getFile(QStringLiteral("provider"), d->mParams.mImage, defaultFile);
}


const QString& ProviderConfigurationInfo::getInternalId() const
{
	return d->mParams.mInternalId;
}
