/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/SignatureChecker.h"

#include "ASN1TemplateUtil.h"
#include "pace/ec/EcUtil.h"

#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


SignatureChecker::SignatureChecker(const QVector<QSharedPointer<const CVCertificate> >& pCertificateChain)
	: mCertificateChain(pCertificateChain)
{
}


bool SignatureChecker::check() const
{
	if (mCertificateChain.isEmpty())
	{
		qCCritical(card) << "Certificate chain is empty";
		return false;
	}

	auto signingCert = mCertificateChain.at(0);
	const EC_KEY* key = signingCert->getBody().getPublicKey().getEcKey();
	if (!key)
	{
		qCCritical(card) << "No elliptic curve parameters";
		return false;
	}

	for (const auto& cert : mCertificateChain)
	{
		if (!checkSignature(cert, signingCert, key))
		{
			qCCritical(card) << "Certificate verification failed:" << cert->getBody().getCertificateHolderReference();
			return false;
		}

		if (cert->getBody().getPublicKey().getEcKey())
		{
			key = cert->getBody().getPublicKey().getEcKey();
		}
		signingCert = cert;
	}

	return true;
}


bool SignatureChecker::checkSignature(const QSharedPointer<const CVCertificate>& pCert, const QSharedPointer<const CVCertificate>& pSigningCert, const EC_KEY* pKey) const
{
	ERR_clear_error();

	// We duplicate the key because we modify it by setting the public point.
	const QSharedPointer<EC_KEY> signingKey = EcUtil::create(EC_KEY_dup(pKey));

	const QByteArray uncompPublicPoint = pSigningCert->getBody().getPublicKey().getUncompressedPublicPoint();
	const auto* const uncompPublicPointData = reinterpret_cast<const unsigned char*>(uncompPublicPoint.constData());
	const auto uncompPublicPointLen = static_cast<size_t>(uncompPublicPoint.size());

	EC_POINT* publicPoint = EC_POINT_new(EC_KEY_get0_group(signingKey.data()));
	const auto guard = qScopeGuard([publicPoint] {
				EC_POINT_free(publicPoint);
			});

	const EC_GROUP* ecGroup = EC_KEY_get0_group(signingKey.data());
	if (!EC_POINT_oct2point(ecGroup, publicPoint, uncompPublicPointData, uncompPublicPointLen, nullptr))
	{
		qCCritical(card) << "Cannot decode uncompressed public point";
		return false;
	}

	if (!EC_KEY_set_public_key(signingKey.data(), publicPoint))
	{
		qCCritical(card) << "Cannot set public point";
		return false;
	}

	const QSharedPointer<EVP_PKEY> evpPkey = EcUtil::create(EVP_PKEY_new());
	if (!EVP_PKEY_set1_EC_KEY(evpPkey.data(), signingKey.data()))
	{
		qCCritical(card) << "Cannot set key";
		return false;
	}

	const QSharedPointer<EVP_PKEY_CTX> ctx = EcUtil::create(EVP_PKEY_CTX_new(evpPkey.data(), nullptr));
	if (!EVP_PKEY_verify_init(ctx.data()))
	{
		qCCritical(card) << "Cannot init verify ctx";
		return false;
	}

	const QByteArray hash = QCryptographicHash::hash(pCert->getRawBody(), pSigningCert->getBody().getHashAlgorithm());
	const QByteArray signature = pCert->getDerSignature();
	const int result = EVP_PKEY_verify(ctx.data(),
			reinterpret_cast<const uchar*>(signature.data()),
			static_cast<size_t>(signature.size()),
			reinterpret_cast<const uchar*>(hash.data()),
			static_cast<size_t>(hash.size()));

	if (result == -1)
	{
		qCCritical(card) << "Signature verification failed, an error occured:" << getOpenSslError();
	}

	return result == 1;
}
