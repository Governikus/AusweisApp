/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderConfigurationInfo.h"

#include "Env.h"
#include "FileProvider.h"

#include <QOperatingSystemVersion>


using namespace governikus;


QString ProviderConfigurationInfo::getDefaultFile(const QString& pSuffix) const
{
#ifdef Q_OS_IOS
	const QString platform;
#else
	const QString platform = QStringLiteral("+android/");
#endif
	const QString category = d->mCategory.isEmpty() ? QStringLiteral("general") : d->mCategory;
	return QStringLiteral(":/images/provider/categoryIcons/%1%2%3.svg").arg(platform, category, pSuffix);
}


ProviderConfigurationInfo::ProviderConfigurationInfo(const LanguageString& pShortName,
		const LanguageString& pLongName,
		const LanguageString& pShortDescription,
		const LanguageString& pLongDescription,
		const QString& pAddress,
		const QString& pHomepage,
		const QString& pCategory,
		const QString& pPhone,
		const QString& pEmail,
		const QString& pPostalAddress,
		const QString& pIcon,
		const QString& pImage,
		const QString& pTcTokenUrl,
		const QString& pClientUrl,
		const QStringList& pSubjectUrls)
	: d(new InternalInfo(pShortName, pLongName, pShortDescription, pLongDescription, pAddress, pHomepage,
			pCategory, pPhone, pEmail, pPostalAddress, pIcon, pImage, pTcTokenUrl, pClientUrl, pSubjectUrls))
{
}


ProviderConfigurationInfo::~ProviderConfigurationInfo()
{
}


void ProviderConfigurationInfo::setTcTokenUrl(const QString& pTcTokenUrl)
{
	d = new InternalInfo(d->mShortName, d->mLongName, d->mShortDescription, d->mLongDescription, d->mAddress, d->mHomepage,
			d->mCategory, d->mPhone, d->mEmail, d->mPostalAddress, d->mIcon, d->mImage, pTcTokenUrl, d->mClientUrl, d->mSubjectUrls);
}


bool ProviderConfigurationInfo::operator ==(const ProviderConfigurationInfo& pOther) const
{
	return *d == *pOther.d;
}


const LanguageString& ProviderConfigurationInfo::getShortName() const
{
	return d->mShortName;
}


const LanguageString& ProviderConfigurationInfo::getLongName() const
{
	return d->mLongName;
}


const LanguageString& ProviderConfigurationInfo::getShortDescription() const
{
	return d->mShortDescription;
}


const LanguageString& ProviderConfigurationInfo::getLongDescription() const
{
	return d->mLongDescription;
}


const QString& ProviderConfigurationInfo::getAddress() const
{
	return d->mAddress;
}


QString ProviderConfigurationInfo::getAddressDomain() const
{
	return QUrl::fromUserInput(d->mAddress).host();
}


const QString& ProviderConfigurationInfo::getHomepage() const
{
	return d->mHomepage;
}


QString ProviderConfigurationInfo::getHomepageBase() const
{
	return QUrl::fromUserInput(d->mHomepage).host();
}


const QString& ProviderConfigurationInfo::getCategory() const
{
	return d->mCategory;
}


const QString& ProviderConfigurationInfo::getPhone() const
{
	return d->mPhone;
}


const QString& ProviderConfigurationInfo::getEMail() const
{
	return d->mEmail;
}


const QString& ProviderConfigurationInfo::getPostalAddress() const
{
	return d->mPostalAddress;
}


QSharedPointer<UpdatableFile> ProviderConfigurationInfo::getIcon() const
{
	const QString defaultFile = getDefaultFile(QStringLiteral("_button"));
	return Env::getSingleton<FileProvider>()->getFile(QStringLiteral("provider"), d->mIcon, defaultFile);
}


QSharedPointer<UpdatableFile> ProviderConfigurationInfo::getImage() const
{
	const QString defaultFile = getDefaultFile(QStringLiteral("_bg"));
	return Env::getSingleton<FileProvider>()->getFile(QStringLiteral("provider"), d->mImage, defaultFile);
}


QUrl ProviderConfigurationInfo::getTcTokenUrl() const
{
	return QUrl::fromUserInput(d->mTcTokenUrl);
}


QUrl ProviderConfigurationInfo::getClientUrl() const
{
	return QUrl::fromUserInput(d->mClientUrl);
}


const QStringList& ProviderConfigurationInfo::getSubjectUrls() const
{
	return d->mSubjectUrls;
}
