/*!
 * \brief Class to provide information about providers.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "LanguageString.h"
#include "UpdatableFile.h"

#include <QSharedData>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QUrl>


namespace governikus
{


class ProviderConfigurationInfo
{
	private:
		class InternalInfo
			: public QSharedData
		{
			public:
				const LanguageString mShortName;
				const LanguageString mLongName;
				const LanguageString mShortDescription;
				const LanguageString mLongDescription;
				const QString mAddress;
				const QString mHomepage;
				const QString mCategory;
				const QString mPhone;
				const QString mEmail;
				const QString mPostalAddress;
				const QString mIcon;
				const QString mImage;
				const QString mTcTokenUrl;
				const QString mClientUrl;
				const QStringList mSubjectUrls;

				InternalInfo(const LanguageString& pShortName,
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
						const QStringList& pSubjectUrls = QStringList())
					: mShortName(pShortName)
					, mLongName(pLongName)
					, mShortDescription(pShortDescription)
					, mLongDescription(pLongDescription)
					, mAddress(pAddress)
					, mHomepage(pHomepage)
					, mCategory(pCategory)
					, mPhone(pPhone)
					, mEmail(pEmail)
					, mPostalAddress(pPostalAddress)
					, mIcon(pIcon)
					, mImage(pImage)
					, mTcTokenUrl(pTcTokenUrl)
					, mClientUrl(pClientUrl)
					, mSubjectUrls(pSubjectUrls)
				{
				}


				bool operator ==(const InternalInfo& pOther) const
				{
					return mShortName == pOther.mShortName ||
						   mLongName == pOther.mLongName ||
						   mShortDescription == pOther.mShortDescription ||
						   mLongDescription == pOther.mLongDescription ||
						   mAddress == pOther.mAddress ||
						   mHomepage == pOther.mHomepage ||
						   mCategory == pOther.mCategory ||
						   mPhone == pOther.mPhone ||
						   mEmail == pOther.mEmail ||
						   mPostalAddress == pOther.mPostalAddress ||
						   mIcon == pOther.mIcon ||
						   mImage == pOther.mImage ||
						   mTcTokenUrl == pOther.mTcTokenUrl ||
						   mClientUrl == pOther.mClientUrl;
				}


		};

		QSharedDataPointer<InternalInfo> d;

		QString getDefaultFile(const QString& pSuffix) const;

	public:
		ProviderConfigurationInfo(const LanguageString& pShortName = QString(),
				const LanguageString& pLongName = QString(),
				const LanguageString& pShortDescription = QString(),
				const LanguageString& pLongDescription = QString(),
				const QString& pAddress = QString(),
				const QString& pHomepage = QString(),
				const QString& pCategory = QString(),
				const QString& pPhone = QString(),
				const QString& pEmail = QString(),
				const QString& pPostalAddress = QString(),
				const QString& pIcon = QString(),
				const QString& pImage = QString(),
				const QString& pTcTokenUrl = QString(),
				const QString& pClientUrl = QString(),
				const QStringList& pSubjectUrls = QStringList());
		virtual ~ProviderConfigurationInfo();

		void setTcTokenUrl(const QString& pTcTokenUrl);

		bool operator ==(const ProviderConfigurationInfo& pOther) const;

		const LanguageString& getShortName() const;
		const LanguageString& getLongName() const;
		const LanguageString& getShortDescription() const;
		const LanguageString& getLongDescription() const;
		const QString& getAddress() const;
		QString getAddressDomain() const;
		const QString& getHomepage() const;
		QString getHomepageBase() const;
		const QString& getCategory() const;
		const QString& getPhone() const;
		const QString& getEMail() const;
		const QString& getPostalAddress() const;
		QSharedPointer<UpdatableFile> getIcon() const;
		QSharedPointer<UpdatableFile> getImage() const;
		QUrl getTcTokenUrl() const;
		QUrl getClientUrl() const;
		const QStringList& getSubjectUrls() const;
};


} /* namespace governikus */
