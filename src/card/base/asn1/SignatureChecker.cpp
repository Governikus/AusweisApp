/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/SignatureChecker.h"
#include "pace/ec/EcUtil.h"

#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


SignatureChecker::SignatureChecker(const QVector<QSharedPointer<const CVCertificate> >& pCertificateChain)
	: mCertificateChain(pCertificateChain)
{
}


bool SignatureChecker::check()
{
	if (mCertificateChain.isEmpty())
	{
		qCCritical(card) << "Certificate chain is empty";
		return false;
	}

	auto signingCert = mCertificateChain.at(0);
	QSharedPointer<const EC_KEY> key = signingCert->getBody().getPublicKey().getEcKey();
	if (!key)
	{
		qCCritical(card) << "No elliptic curve parameters";
		return false;
	}

	for (const auto& cert : mCertificateChain)
	{
		if (!checkSignature(cert, signingCert, key.data()))
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


bool SignatureChecker::checkSignature(const QSharedPointer<const CVCertificate>& pCert, const QSharedPointer<const CVCertificate>& pSigningCert, const EC_KEY* pKey)
{
	// We duplicate the key because we modify it by setting the public point.
	QSharedPointer<EC_KEY> signingKey = EcUtil::create(EC_KEY_dup(pKey));


	QByteArray uncompPublicPoint = pSigningCert->getBody().getPublicKey().getUncompressedPublicPoint();
	const unsigned char* uncompPublicPointData = reinterpret_cast<const unsigned char*>(uncompPublicPoint.constData());
	size_t uncompPublicPointLen = static_cast<size_t>(uncompPublicPoint.size());

	EC_POINT* publicPoint = EC_POINT_new(EC_KEY_get0_group(signingKey.data()));
	const EC_GROUP* ecGroup = EC_KEY_get0_group(signingKey.data());
	if (!EC_POINT_oct2point(ecGroup, publicPoint, uncompPublicPointData, uncompPublicPointLen, nullptr))
	{
		qCCritical(card) << "Cannot decode uncompressed public point";
		return false;
	}
	EC_KEY_set_public_key(signingKey.data(), publicPoint);

	QByteArray bodyHash = QCryptographicHash::hash(pCert->getRawBody(), pSigningCert->getBody().getHashAlgorithm());
	const unsigned char* dgst = reinterpret_cast<const unsigned char*>(bodyHash.constData());
	int dgstlen = bodyHash.size();

	int result = ECDSA_do_verify(dgst, dgstlen, pCert->getEcdsaSignature().data(), signingKey.data());

	if (result == -1)
	{
		ERR_load_crypto_strings();
		qCCritical(card) << "Signature verification failed, an error occured: " << ERR_error_string(ERR_get_error(), nullptr);
	}
	return(result == 1);
}
