/*!
 * \brief Utility class that provides access to the "secure storage" of the application, which contains
 * the certificates for preverification and update checks.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
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

		QJsonArray readJsonArray(const QJsonObject& pConfig, const QLatin1String pName) const;
		QString readGroup(const QJsonObject& pConfig, const QLatin1String pGroup, const QLatin1String pName) const;
		QMap<QSsl::KeyAlgorithm, int> readKeySizes(const QJsonObject& pConfig, const QLatin1String pKey) const;
		QByteArrayList readByteArrayList(const QJsonObject& pConfig, const QLatin1String pName) const;

		QJsonObject loadFile(const QStringList& pFiles) const;
		void load();

		QByteArrayList loadTestCvcsFromAppDir();
		QByteArray loadTestCvc(const QString& pPath) const;

	protected:
		SecureStorage();
		virtual ~SecureStorage();
		static SecureStorage& getInstance();

	public:
		enum class TlsSuite
		{
			DEFAULT, PSK,
		};

		const QByteArrayList& getCVRootCertificates(bool pProductive) const;
		const QVector<QSslCertificate>& getUpdateCertificates() const;
		const QUrl& getSelfAuthenticationUrl(bool pTest = false) const;
		const QUrl& getUpdateServerBaseUrl() const;
		const QUrl& getWhitelistServerBaseUrl() const;
		const QUrl& getAppcastUpdateUrl() const;
		const QUrl& getAppcastBetaUpdateUrl() const;
		const TlsConfiguration& getTlsConfig(TlsSuite pTlsSuite = TlsSuite::DEFAULT) const;
		const TlsConfiguration& getTlsConfigRemote(TlsSuite pTlsSuite = TlsSuite::DEFAULT) const;
		int getMinimumStaticKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const;
		int getMinimumEphemeralKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const;
		bool isLoaded() const;
};


} // namespace governikus
