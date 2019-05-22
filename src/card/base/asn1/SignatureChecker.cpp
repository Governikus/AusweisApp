/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/SignatureChecker.h"

#include "ASN1TemplateUtil.h"
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
	ERR_clear_error();

	// We duplicate the key because we modify it by setting the public point.
	const QSharedPointer<EC_KEY> signingKey = EcUtil::create(EC_KEY_dup(pKey));

	const QByteArray uncompPublicPoint = pSigningCert->getBody().getPublicKey().getUncompressedPublicPoint();
	const unsigned char* uncompPublicPointData = reinterpret_cast<const unsigned char*>(uncompPublicPoint.constData());
	const auto uncompPublicPointLen = static_cast<size_t>(uncompPublicPoint.size());

	EC_POINT* publicPoint = EC_POINT_new(EC_KEY_get0_group(signingKey.data()));
	const EC_GROUP* ecGroup = EC_KEY_get0_group(signingKey.data());
	if (!EC_POINT_oct2point(ecGroup, publicPoint, uncompPublicPointData, uncompPublicPointLen, nullptr))
	{
		qCCritical(card) << "Cannot decode uncompressed public point";
		return false;
	}
	EC_KEY_set_public_key(signingKey.data(), publicPoint);

	const QByteArray bodyHash = QCryptographicHash::hash(pCert->getRawBody(), pSigningCert->getBody().getHashAlgorithm());
	const unsigned char* dgst = reinterpret_cast<const unsigned char*>(bodyHash.constData());
	const int dgstlen = bodyHash.size();
	const int result = ECDSA_do_verify(dgst, dgstlen, pCert->getEcdsaSignature().data(), signingKey.data());

	if (result == -1)
	{
		qCCritical(card) << "Signature verification failed, an error occured:" << getOpenSslError();
	}

	return result == 1;
}
