/*!
 * \brief Class to provide information about providers.
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
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
				const LanguageString mLongDescription;
				const QString mAddress;
				const QString mHomepage;
				const QString mCategory;
				const QString mPhone;
				const QString mEmail;
				const QString mPostalAddress;
				const QString mIcon;
				const QString mImage;
				const QStringList mSubjectUrls;
				const QString mSubjectUrlInfo;
				const QString mInternalId;

				InternalInfo(const LanguageString& pShortName,
						const LanguageString& pLongName,
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
						const QString& pSubjectUrlInfo,
						const QString& pInternalId)
					: mShortName(pShortName)
					, mLongName(pLongName)
					, mLongDescription(pLongDescription)
					, mAddress(pAddress)
					, mHomepage(pHomepage)
					, mCategory(pCategory)
					, mPhone(pPhone)
					, mEmail(pEmail)
					, mPostalAddress(pPostalAddress)
					, mIcon(pIcon)
					, mImage(pImage)
					, mSubjectUrls(pSubjectUrls)
					, mSubjectUrlInfo(pSubjectUrlInfo)
					, mInternalId(pInternalId)
				{
				}


				bool operator ==(const InternalInfo& pOther) const
				{
					return mShortName == pOther.mShortName &&
						   mLongName == pOther.mLongName &&
						   mLongDescription == pOther.mLongDescription &&
						   mAddress == pOther.mAddress &&
						   mHomepage == pOther.mHomepage &&
						   mCategory == pOther.mCategory &&
						   mPhone == pOther.mPhone &&
						   mEmail == pOther.mEmail &&
						   mPostalAddress == pOther.mPostalAddress &&
						   mIcon == pOther.mIcon &&
						   mImage == pOther.mImage &&
						   mSubjectUrls == pOther.mSubjectUrls &&
						   mSubjectUrlInfo == pOther.mSubjectUrlInfo &&
						   mInternalId == pOther.mInternalId;
				}


		};

		QSharedDataPointer<InternalInfo> d;

		[[nodiscard]] QString getDefaultFile(const QString& pSuffix) const;

	public:
		ProviderConfigurationInfo(const LanguageString& pShortName = QString(),
				const LanguageString& pLongName = QString(),
				const LanguageString& pLongDescription = QString(),
				const QString& pAddress = QString(),
				const QString& pHomepage = QString(),
				const QString& pCategory = QString(),
				const QString& pPhone = QString(),
				const QString& pEmail = QString(),
				const QString& pPostalAddress = QString(),
				const QString& pIcon = QString(),
				const QString& pImage = QString(),
				const QStringList& pSubjectUrls = QStringList(),
				const QString& pSubjectUrlInfo = QString(),
				const QString& pInternalId = QString());
		virtual ~ProviderConfigurationInfo();

		bool operator ==(const ProviderConfigurationInfo& pOther) const;
		bool operator !=(const ProviderConfigurationInfo& pOther) const;
		[[nodiscard]] bool matchWithSubjectUrl(const QString& pSubjectUrl) const;

		[[nodiscard]] const LanguageString& getShortName() const;
		[[nodiscard]] const LanguageString& getLongName() const;
		[[nodiscard]] const LanguageString& getLongDescription() const;
		[[nodiscard]] const QString& getAddress() const;
		[[nodiscard]] QString getAddressDomain() const;
		[[nodiscard]] const QString& getHomepage() const;
		[[nodiscard]] QString getHomepageBase() const;
		[[nodiscard]] const QString& getCategory() const;
		[[nodiscard]] const QString& getPhone() const;
		[[nodiscard]] const QString& getEMail() const;
		[[nodiscard]] const QString& getPostalAddress() const;
		[[nodiscard]] QSharedPointer<UpdatableFile> getIcon() const;
		[[nodiscard]] QSharedPointer<UpdatableFile> getImage() const;
		[[nodiscard]] const QStringList& getSubjectUrls() const;
		[[nodiscard]] const QString& getSubjectUrlInfo() const;
		[[nodiscard]] const QString& getInternalId() const;
};


} // namespace governikus
