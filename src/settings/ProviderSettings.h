/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractSettings.h"
#include "LanguageString.h"

#include <QDateTime>
#include <QString>
#include <QVector>


class test_ProviderSettings;


namespace governikus
{


class Provider
{
	friend bool operator==(const Provider& pLeft, const Provider& pRight);

	private:
		LanguageString mShortName;
		LanguageString mLongName;
		LanguageString mShortDescription;
		LanguageString mLongDescription;
		QString mAddress;
		QString mHomepage;
		QString mCategory;
		QString mPhone;
		QString mEmail;
		QString mPostalAddress;
		QString mIcon;
		QString mImage;
		QString mLocalIconUrl;
		QString mLocalImageUrl;
		QString mTcTokenUrl;
		QString mClientUrl;

	public:
		Provider(const LanguageString& pShortName = QString(),
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
				const QString& pClientUrl = QString());

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
		const QString& getIcon() const;
		const QString& getImage() const;
		const QString& getIconUrl() const;
		const QString& getImageUrl() const;
		const QString& getTcTokenUrl() const;
		const QString& getClientUrl() const;

		void setTcTokenUrl(const QString& pTcTokenUrl);

		// Use pMap to update icon and image urls in this provider.
		void applyMap(const QMap<QString, QString>& pMap);
};

inline bool operator==(const Provider& pLeft, const Provider& pRight)
{
	return &pLeft == &pRight || (
		pLeft.mShortName == pRight.mShortName &&
		pLeft.mLongName == pRight.mLongName &&
		pLeft.mShortDescription == pRight.mShortDescription &&
		pLeft.mLongDescription == pRight.mLongDescription &&
		pLeft.mAddress == pRight.mAddress &&
		pLeft.mHomepage == pRight.mHomepage &&
		pLeft.mCategory == pRight.mCategory &&
		pLeft.mPhone == pRight.mPhone &&
		pLeft.mEmail == pRight.mEmail &&
		pLeft.mPostalAddress == pRight.mPostalAddress &&
		pLeft.mIcon == pRight.mIcon &&
		pLeft.mImage == pRight.mImage &&
		pLeft.mTcTokenUrl == pRight.mTcTokenUrl &&
		pLeft.mClientUrl == pRight.mClientUrl);

	// since those variable won't be saved we don't need to check!
	// pLeft.mLocalIconUrl == pRight.mLocalIconUrl &&
	// pLeft.mLocalImageUrl == pRight.mLocalImageUrl
}


class ProviderSettings
	: public AbstractSettings
{
	Q_OBJECT

	friend class AppSettings;
	friend class ProviderParser;
	friend class ::test_ProviderSettings;
	friend bool operator==(const ProviderSettings& pLeft, const ProviderSettings& pRight);

	private:
		QDateTime mIssueDate;
		QVector<Provider> mProviders;
		QMap<QString, QString> mIconMap;

		// Use mIconMap to update icon and image urls in all providers.
		void applyMap();

		QStringList getRequiredIcons() const;

		ProviderSettings();
		ProviderSettings(const QDateTime& pIssueDate, const QVector<Provider>& pProviders);

		static void save(QSharedPointer<QSettings>& pSettings, const QString& pGroupName, LanguageString const& pLanguageString);
		static LanguageString load(const QSharedPointer<QSettings>& pSettings, const QString& pGroupName);

	public:
		virtual ~ProviderSettings();

		virtual void load() override;

		virtual bool isUnsaved() const override;

		virtual void save() override;

		virtual void update(const AbstractSettings& pOther) override;

		const QDateTime& getIssueDate() const;
		void setIssueDate(const QDateTime& pIssueDate);

		const QVector<Provider>& getProviders() const;
		void setProviders(const QVector<Provider>& pProviders);

		void requestIconMap();

	public Q_SLOTS:
		void onIconMapChanged(const QMap<QString, QString>& pIconMap);

	Q_SIGNALS:
		void fireIssueDateChanged();

		void fireProvidersChanged();

		void fireRequiredIcons(const QStringList& pIcons);
};

inline bool operator==(const ProviderSettings& pLeft, const ProviderSettings& pRight)
{
	return &pLeft == &pRight || (
		pLeft.mIssueDate == pRight.mIssueDate &&
		pLeft.mProviders == pRight.mProviders &&
		pLeft.mIconMap == pRight.mIconMap);
}


inline bool operator!=(const ProviderSettings& pLeft, const ProviderSettings& pRight)
{
	return !(pLeft == pRight);
}


} /* namespace governikus */
