/*
 * \brief Generates a new private/public key with an X509 certificate.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
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
		static EVP_PKEY* createKey();

	public:
		static KeyPair generate();

		const QSslKey& getKey() const;
		const QSslCertificate& getCertificate() const;
		bool isValid() const;
};


} // namespace governikus
