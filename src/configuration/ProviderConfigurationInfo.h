/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
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

struct ProviderConfigurationInfoParams
{
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
	const QString mInternalId;

	bool operator ==(const ProviderConfigurationInfoParams& pOther) const
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
			   mInternalId == pOther.mInternalId;
	}


};

class ProviderConfigurationInfo
{
	private:
		class InternalInfo
			: public QSharedData
		{
			public:
				const ProviderConfigurationInfoParams mParams;

				explicit InternalInfo(const ProviderConfigurationInfoParams& pInfos)
					: mParams(pInfos)
				{
				}


				bool operator ==(const InternalInfo& pOther) const
				{
					return mParams == pOther.mParams;
				}


		};

		QSharedDataPointer<InternalInfo> d;

		[[nodiscard]] QString getDefaultFile(const QString& pSuffix) const;

	public:
		explicit ProviderConfigurationInfo(const ProviderConfigurationInfoParams& pParams = {});
		virtual ~ProviderConfigurationInfo();

		bool operator ==(const ProviderConfigurationInfo& pOther) const;
		bool operator !=(const ProviderConfigurationInfo& pOther) const;

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
		[[nodiscard]] const QString& getInternalId() const;
};


} // namespace governikus
