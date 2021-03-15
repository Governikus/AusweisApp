/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderConfigurationInfo.h"

#include "FileProvider.h"

#include <QOperatingSystemVersion>


using namespace governikus;


QString ProviderConfigurationInfo::getDefaultFile(const QString& pSuffix) const
{
#ifdef Q_OS_IOS
	const QString platform = QStringLiteral("ios/");
#else
	const QString platform;
#endif
	const QString category = d->mCategory.isEmpty() ? QStringLiteral("general") : d->mCategory;
	return QStringLiteral(":/images/provider/%1%2%3.svg").arg(platform, category, pSuffix);
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
		const QStringList& pSubjectUrls,
		const QString& pSubjectUrlInfo)
	: d(new InternalInfo(pShortName, pLongName, pShortDescription, pLongDescription, pAddress, pHomepage,
			pCategory, pPhone, pEmail, pPostalAddress, pIcon, pImage, pSubjectUrls, pSubjectUrlInfo))
{
}


ProviderConfigurationInfo::~ProviderConfigurationInfo()
{
}


bool ProviderConfigurationInfo::operator ==(const ProviderConfigurationInfo& pOther) const
{
	return *d == *pOther.d;
}


bool ProviderConfigurationInfo::matchWithSubjectUrl(const QString& pSubjectUrl) const
{
	if (pSubjectUrl.isEmpty())
	{
		return false;
	}
	const QString subjectUrlHost = QUrl(pSubjectUrl).host();

	// Check provider address host.
	if (!getAddress().isEmpty() && QUrl(getAddress()).host() == subjectUrlHost)
	{
		return true;
	}

	// Check subject urls.
	for (const auto& subjectUrl : getSubjectUrls())
	{
		if (!subjectUrl.isEmpty() && QUrl(subjectUrl).host() == subjectUrlHost)
		{
			return true;
		}
	}

	return false;
}


const LanguageString& ProviderConfigurationInfo::getShortName() const
{
	return d->mShortName;
}


const LanguageString& ProviderConfigurationInfo::getLongName() const
{
	return !d->mLongName.toString().isNull() ? d->mLongName : getShortName();
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


const QStringList& ProviderConfigurationInfo::getSubjectUrls() const
{
	return d->mSubjectUrls;
}


const QString& ProviderConfigurationInfo::getSubjectUrlInfo() const
{
	return d->mSubjectUrlInfo;
}
