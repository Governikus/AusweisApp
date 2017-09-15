/*!
 * SecureStorage.h
 *
 * \brief Utility class that provides access to the "secure storage" of the application, which contains
 * the certificates for preverification and update checks.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractSettings.h"
#include "TlsSettings.h"

#include <QByteArrayList>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QSslCertificate>
#include <QSslCipher>
#include <QString>
#include <QUrl>
#include <QVector>


class test_SecureStorage;


namespace governikus
{

using SignatureAlgorithmPair = QPair<QSsl::KeyAlgorithm, QCryptographicHash::Algorithm>;

class SecureStorage final
{
	friend class AppSettings;
	friend class ::test_SecureStorage;
	friend bool operator==(const SecureStorage& pLeft, const SecureStorage& pRight);

	private:
		QDateTime mLoadedTime;
		QByteArrayList mCvcas;
		QByteArrayList mCvcasTest;
		QVector<QSslCertificate> mUpdateCertificates;
		QUrl mSelfAuthenticationUrl;
		QUrl mSelfAuthenticationTestUrl;
		QUrl mDriverUpdateUrl;
		QUrl mProviderUpdateUrl;
		QString mProviderIconUpdateUrlBase;
		QByteArray mSelfAuthenticationCertDescr;
		QByteArray mSelfAuthenticationTestCertDescr;
		QUrl mAppcastUpdateUrl;
		QUrl mAppcastBetaUpdateUrl;

		TlsSettings mTlsSettings, mTlsSettingsPsk;
		QMap<QSsl::KeyAlgorithm, int> mMinStaticKeySizes;
		QMap<QSsl::KeyAlgorithm, int> mMinEphemeralKeySizes;

		SecureStorage();
		~SecureStorage();

		bool readJsonArray(QJsonArray& pArray, const QJsonObject& pConfig, const QLatin1String& pName);
		QString readGroup(const QJsonObject& pConfig, const QLatin1String& pGroup, const QLatin1String& pName);
		QMap<QSsl::KeyAlgorithm, int> readKeySizes(const QJsonObject& pConfig, const QLatin1String& pKey);
		void readByteArrayList(QByteArrayList& pArray, const QJsonObject& pConfig, const QLatin1String& pName);

	public:
		void load();

		const QByteArrayList& getCVRootCertificates(bool pProductive) const;
		const QVector<QSslCertificate>& getUpdateCertificates() const;
		const QUrl& getSelfAuthenticationUrl(bool pTest = false) const;
		const QByteArray& getSelfAuthenticationCertDescr(bool pTest = false) const;
		const QUrl& getDriverUpdateUrl() const;
		const QUrl& getProviderUpdateUrl() const;
		const QString& getProviderIconUpdateUrlBase() const;
		const QUrl& getAppcastUpdateUrl() const;
		const QUrl& getAppcastBetaUpdateUrl() const;
		const TlsSettings& getTlsSettings() const;
		const TlsSettings& getTlsSettingsPsk() const;
		int getMinimumStaticKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const;
		int getMinimumEphemeralKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const;
};

inline bool operator==(const SecureStorage& pLeft, const SecureStorage& pRight)
{
	return &pLeft == &pRight || (
		containsAllEntries(pLeft.mCvcas, pRight.mCvcas) &&
		containsAllEntries(pLeft.mCvcasTest, pRight.mCvcasTest) &&
		containsAllEntries(pLeft.mUpdateCertificates, pRight.mUpdateCertificates) &&
		pLeft.mSelfAuthenticationUrl == pRight.mSelfAuthenticationUrl &&
		pLeft.mSelfAuthenticationTestUrl == pRight.mSelfAuthenticationTestUrl &&
		pLeft.mDriverUpdateUrl == pRight.mDriverUpdateUrl &&
		pLeft.mProviderUpdateUrl == pRight.mProviderUpdateUrl &&
		pLeft.mProviderIconUpdateUrlBase == pRight.mProviderIconUpdateUrlBase &&
		pLeft.mSelfAuthenticationCertDescr == pRight.mSelfAuthenticationCertDescr &&
		pLeft.mSelfAuthenticationTestCertDescr == pRight.mSelfAuthenticationTestCertDescr &&
		pLeft.mAppcastUpdateUrl == pRight.mAppcastUpdateUrl &&
		pLeft.mAppcastBetaUpdateUrl == pRight.mAppcastBetaUpdateUrl &&
		pLeft.mTlsSettings == pRight.mTlsSettings &&
		pLeft.mTlsSettingsPsk == pRight.mTlsSettingsPsk &&
		pLeft.mMinStaticKeySizes == pRight.mMinStaticKeySizes &&
		pLeft.mMinEphemeralKeySizes == pRight.mMinEphemeralKeySizes);
}


inline bool operator!=(const SecureStorage& pLeft, const SecureStorage& pRight)
{
	return !(pLeft == pRight);
}


} // namespace governikus
