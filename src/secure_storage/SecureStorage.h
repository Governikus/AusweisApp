/*!
 * \brief Utility class that provides access to the "secure storage" of the application, which contains
 * the certificates for preverification and update checks.
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
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

	private:
		bool mLoaded;
		QByteArrayList mCvcas;
		QByteArrayList mCvcasTest;
		QVector<QSslCertificate> mUpdateCertificates;
		QUrl mSelfAuthenticationUrl;
		QUrl mSelfAuthenticationTestUrl;
		QUrl mUpdateServerBaseUrl;
		QUrl mWhitelistServerBaseUrl;
		QUrl mAppcastUpdateUrl;
		QUrl mAppcastBetaUpdateUrl;

		TlsConfiguration mTlsConfig, mTlsConfigPsk, mTlsConfigRemote, mTlsConfigRemotePsk;
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
		virtual ~SecureStorage();
		static SecureStorage& getInstance();

	public:
		enum class TlsSuite
		{
			DEFAULT, PSK,
		};

		[[nodiscard]] const QByteArrayList& getCVRootCertificates(bool pProductive) const;
		[[nodiscard]] const QVector<QSslCertificate>& getUpdateCertificates() const;
		[[nodiscard]] const QUrl& getSelfAuthenticationUrl(bool pTest = false) const;
		[[nodiscard]] const QUrl& getUpdateServerBaseUrl() const;
		[[nodiscard]] const QUrl& getWhitelistServerBaseUrl() const;
		[[nodiscard]] const QUrl& getAppcastUpdateUrl() const;
		[[nodiscard]] const QUrl& getAppcastBetaUpdateUrl() const;
		[[nodiscard]] const TlsConfiguration& getTlsConfig(TlsSuite pTlsSuite = TlsSuite::DEFAULT) const;
		[[nodiscard]] const TlsConfiguration& getTlsConfigRemote(TlsSuite pTlsSuite = TlsSuite::DEFAULT) const;
		[[nodiscard]] int getMinimumStaticKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const;
		[[nodiscard]] int getMinimumEphemeralKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const;
		[[nodiscard]] bool isLoaded() const;
};


} // namespace governikus
