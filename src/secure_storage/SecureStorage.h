/*!
 * \brief Utility class that provides access to the "secure storage" of the application, which contains
 * the certificates for preverification and update checks.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
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
class MockSecureStorage;

namespace governikus
{

class SecureStorage
{
	Q_GADGET

	friend class Env;
	friend class ::MockSecureStorage;
	friend class ::test_SecureStorage;

	private:
		bool mLoaded;
		QString mVendor;
		QByteArrayList mCvcas;
		QByteArrayList mCvcasTest;
		QVector<QSslCertificate> mUpdateCertificates;
		QUrl mSelfAuthenticationUrl;
		QUrl mSelfAuthenticationTestUrl;
		QUrl mUpdateServerBaseUrl;
		QUrl mWhitelistServerBaseUrl;
		QUrl mAppcastUpdateUrl;
		QUrl mAppcastBetaUpdateUrl;
		QString mSmartPersonalizationUrl;
		QString mSmartPersonalizationTestUrl;
		QString mSmartServiceId;
		QString mSmartVersionTag;
		QString mSmartSsdAid;
		QString mLocalIfdPackageName;
		QString mLocalIfdMinVersion;
		QByteArrayList mLocalIfAllowedCertificateHashes;
		int mLocalIfdMinPskSize;

		TlsConfiguration mTlsConfig;
		TlsConfiguration mTlsConfigPsk;
		TlsConfiguration mTlsConfigRemoteIfd;
		TlsConfiguration mTlsConfigRemoteIfdPairing;
		TlsConfiguration mTlsConfigLocalIfd;
		QMap<QSsl::KeyAlgorithm, int> mMinStaticKeySizes;
		QMap<QSsl::KeyAlgorithm, int> mMinEphemeralKeySizes;

		[[nodiscard]] QJsonArray readJsonArray(const QJsonObject& pConfig, const QLatin1String pName) const;
		[[nodiscard]] QString readGroup(const QJsonObject& pConfig, const QLatin1String pGroup, const QLatin1String pName) const;
		[[nodiscard]] QMap<QSsl::KeyAlgorithm, int> readKeySizes(const QJsonObject& pConfig, const QLatin1String pKey) const;
		[[nodiscard]] QByteArrayList readByteArrayList(const QJsonObject& pConfig, const QLatin1String pName) const;

		[[nodiscard]] QJsonObject loadFile(const QStringList& pFiles) const;
		void load();

		QByteArrayList loadTestCvcsFromAppDir();
		[[nodiscard]] QByteArray loadTestCvc(const QString& pPath) const;

	protected:
		SecureStorage();
		~SecureStorage() = default;
		static SecureStorage& getInstance();

	public:
		enum class TlsSuite
		{
			DEFAULT, PSK,
		};

		[[nodiscard]] const QString& getVendor() const;
		[[nodiscard]] const QByteArrayList& getCVRootCertificates(bool pProductive) const;
		[[nodiscard]] const QVector<QSslCertificate>& getUpdateCertificates() const;
		[[nodiscard]] const QUrl& getSelfAuthenticationUrl(bool pTest = false) const;
		[[nodiscard]] const QUrl& getUpdateServerBaseUrl() const;
		[[nodiscard]] const QUrl& getWhitelistServerBaseUrl() const;
		[[nodiscard]] const QUrl& getAppcastUpdateUrl() const;
		[[nodiscard]] const QUrl& getAppcastBetaUpdateUrl() const;
		[[nodiscard]] const QString& getSmartPersonalizationUrl(bool pTest = false) const;
		[[nodiscard]] const QString& getSmartServiceId() const;
		[[nodiscard]] const QString& getSmartVersionTag() const;
		[[nodiscard]] const QString& getSmartSsdAid() const;
		[[nodiscard]] const QString& getLocalIfdPackageName() const;
		[[nodiscard]] const QString& getLocalIfdMinVersion() const;
		[[nodiscard]] const QByteArrayList& getLocalIfdAllowedCertificateHashes() const;
		[[nodiscard]] int getLocalIfdMinPskSize() const;
		[[nodiscard]] const TlsConfiguration& getTlsConfig(TlsSuite pTlsSuite = TlsSuite::DEFAULT) const;
		[[nodiscard]] const TlsConfiguration& getTlsConfigRemoteIfd(TlsSuite pTlsSuite = TlsSuite::DEFAULT) const;
		[[nodiscard]] const TlsConfiguration& getTlsConfigLocalIfd() const;
		[[nodiscard]] int getMinimumStaticKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const;
		[[nodiscard]] int getMinimumEphemeralKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const;
		[[nodiscard]] bool isLoaded() const;

		[[nodiscard]] QString getDeveloperConfig() const;
		[[nodiscard]] QString getCustomConfig() const;
		[[nodiscard]] QString getEmbeddedConfig() const;
};


} // namespace governikus
