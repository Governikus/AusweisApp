/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
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
		static QSharedPointer<X509> createCertificate(EVP_PKEY* pPkey);
		static EVP_PKEY* createKey(const char* pCurve);

	public:
		static KeyPair generate(const char* pCurve = nullptr);

		[[nodiscard]] const QSslKey& getKey() const;
		[[nodiscard]] const QSslCertificate& getCertificate() const;
		[[nodiscard]] bool isValid() const;
};


} // namespace governikus
