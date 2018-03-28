/*!
 * \brief Utility class that provides access to the "secure storage" of the application, which contains
 * the certificates for preverification and update checks.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "TlsConfiguration.h"

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

class SecureStorage
{
	friend class ::test_SecureStorage;

	private:
		QDateTime mLoadedTime;
		QByteArrayList mCvcas;
		QByteArrayList mCvcasTest;
		QVector<QSslCertificate> mUpdateCertificates;
		QUrl mSelfAuthenticationUrl;
		QUrl mSelfAuthenticationTestUrl;
		QUrl mUpdateServerBaseUrl;
		QUrl mAppcastUpdateUrl;
		QUrl mAppcastBetaUpdateUrl;

		TlsConfiguration mTlsConfig, mTlsConfigPsk, mTlsConfigRemote, mTlsConfigRemotePsk;
		QMap<QSsl::KeyAlgorithm, int> mMinStaticKeySizes;
		QMap<QSsl::KeyAlgorithm, int> mMinEphemeralKeySizes;

		bool readJsonArray(QJsonArray& pArray, const QJsonObject& pConfig, const QLatin1String pName);
		QString readGroup(const QJsonObject& pConfig, const QLatin1String pGroup, const QLatin1String pName);
		QMap<QSsl::KeyAlgorithm, int> readKeySizes(const QJsonObject& pConfig, const QLatin1String pKey);
		void readByteArrayList(QByteArrayList& pArray, const QJsonObject& pConfig, const QLatin1String pName);

		void load();

	protected:
		SecureStorage();
		virtual ~SecureStorage();

	public:
		static SecureStorage& getInstance();

		enum class TlsSuite
		{
			DEFAULT, PSK,
		};

		const QByteArrayList& getCVRootCertificates(bool pProductive) const;
		const QVector<QSslCertificate>& getUpdateCertificates() const;
		const QUrl& getSelfAuthenticationUrl(bool pTest = false) const;
		const QUrl& getUpdateServerBaseUrl() const;
		const QUrl& getAppcastUpdateUrl() const;
		const QUrl& getAppcastBetaUpdateUrl() const;
		const TlsConfiguration& getTlsConfig(TlsSuite pTlsSuite = TlsSuite::DEFAULT) const;
		const TlsConfiguration& getTlsConfigRemote(TlsSuite pTlsSuite = TlsSuite::DEFAULT) const;
		int getMinimumStaticKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const;
		int getMinimumEphemeralKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const;
};


} // namespace governikus
