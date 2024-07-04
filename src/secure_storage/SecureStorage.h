/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Utility class that provides access to the "secure storage" of the application, which contains
 * the certificates for preverification and update checks.
 */

#pragma once

#include "Env.h"
#include "TlsConfiguration.h"

#include <QByteArrayList>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QSslCertificate>
#include <QSslCipher>
#include <QString>
#include <QUrl>


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
		QString mVendor;
		QByteArrayList mCvcas;
		QByteArrayList mCvcasTest;
		QList<QSslCertificate> mUpdateCertificates;
		QUrl mSelfAuthenticationUrl;
		QUrl mSelfAuthenticationTestUrl;
		QUrl mUpdateServerBaseUrl;
		QUrl mWhitelistServerBaseUrl;
		QUrl mAppcastUpdateUrl;
		QUrl mAppcastBetaUpdateUrl;
		QString mSmartPersonalizationUrl;
		QString mSmartPersonalizationTestUrl;
		QString mSmartServiceId;
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
		QMap<QSsl::KeyAlgorithm, int> mMinKeySizes;
		QMap<QSsl::KeyAlgorithm, int> mMinKeySizesIfd;
		int mCreateKeySizeIfd;

		[[nodiscard]] QJsonArray readJsonArray(const QJsonObject& pConfig, const QLatin1String pName) const;
		[[nodiscard]] QString readGroup(const QJsonObject& pConfig, const QLatin1String pGroup, const QLatin1String pName) const;
		[[nodiscard]] QMap<QSsl::KeyAlgorithm, int> readKeySizes(const QJsonObject& pConfig, const QLatin1String pKey) const;
		[[nodiscard]] QByteArrayList readByteArrayList(const QJsonObject& pConfig, const QLatin1String pName) const;

		[[nodiscard]] QJsonObject loadFile(const QStringList& pFiles) const;
		void load();

		QByteArrayList loadTestCvcsFromAppDir() const;
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
		[[nodiscard]] const QList<QSslCertificate>& getUpdateCertificates() const;
		[[nodiscard]] const QUrl& getSelfAuthenticationUrl(bool pTest = false) const;
		[[nodiscard]] const QUrl& getUpdateServerBaseUrl() const;
		[[nodiscard]] const QUrl& getWhitelistServerBaseUrl() const;
		[[nodiscard]] const QUrl& getAppcastUpdateUrl() const;
		[[nodiscard]] const QUrl& getAppcastBetaUpdateUrl() const;
		[[nodiscard]] const QString& getSmartPersonalizationUrl(bool pTest = false) const;
		[[nodiscard]] const QString& getSmartServiceId() const;
		[[nodiscard]] const QString& getSmartSsdAid() const;
		[[nodiscard]] const QString& getLocalIfdPackageName() const;
		[[nodiscard]] const QString& getLocalIfdMinVersion() const;
		[[nodiscard]] const QByteArrayList& getLocalIfdAllowedCertificateHashes() const;
		[[nodiscard]] int getLocalIfdMinPskSize() const;
		[[nodiscard]] const TlsConfiguration& getTlsConfig(TlsSuite pTlsSuite = TlsSuite::DEFAULT) const;
		[[nodiscard]] const TlsConfiguration& getTlsConfigRemoteIfd(TlsSuite pTlsSuite = TlsSuite::DEFAULT) const;
		[[nodiscard]] const TlsConfiguration& getTlsConfigLocalIfd() const;
		[[nodiscard]] int getMinimumKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const;
		[[nodiscard]] int getMinimumIfdKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const;
		[[nodiscard]] int getIfdCreateSize() const;
		[[nodiscard]] bool isValid() const;

		[[nodiscard]] QString getDeveloperConfig() const;
		[[nodiscard]] QString getCustomConfig() const;
		[[nodiscard]] QString getCustomConfigPath() const;
		[[nodiscard]] QString getEmbeddedConfig() const;
};


} // namespace governikus
