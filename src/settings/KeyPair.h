/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*
 * \brief Generates a new private/public key with an X509 certificate.
 */

#pragma once

#include <openssl/evp.h>
#include <openssl/x509.h>

#include <QByteArray>
#include <QSharedPointer>
#include <QSslCertificate>
#include <QSslKey>

namespace governikus
{

class KeyPair
{
	private:
		const QSslKey mKey;
		const QSslCertificate mCertificate;

		KeyPair() = default;
		KeyPair(const QSslKey& pKey, const QSslCertificate& pCert);

		static QByteArray rewriteCertificate(X509* pX509);
		static QSharedPointer<X509> createCertificate(EVP_PKEY* pPkey, const QSharedPointer<EVP_PKEY>& pSignerKey = nullptr, const QSharedPointer<X509>& pSignerCert = nullptr);
		static EVP_PKEY* createKey(int pKeyCtxNid, const std::function<bool(EVP_PKEY_CTX*)>& pFunc);
		static QSharedPointer<EVP_PKEY> parseKey(const QByteArray& pData);
		static QSharedPointer<X509> parseCertificate(const QByteArray& pData);
		static KeyPair generatePair(EVP_PKEY* pKey, const QByteArray& pSignerKey, const QByteArray& pSignerCert);

	public:
		static KeyPair generate(const char* pCurve, const QByteArray& pSignerKey = QByteArray(), const QByteArray& pSignerCert = QByteArray());
		static KeyPair generate(int pKeySize = 2048, const QByteArray& pSignerKey = QByteArray(), const QByteArray& pSignerCert = QByteArray());

		[[nodiscard]] const QSslKey& getKey() const;
		[[nodiscard]] const QSslCertificate& getCertificate() const;
		[[nodiscard]] bool isValid() const;
};


} // namespace governikus
