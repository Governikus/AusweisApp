/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/SignatureChecker.h"

#include "ASN1TemplateUtil.h"
#include "pace/ec/EcUtil.h"

#include <QLoggingCategory>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <openssl/evp.h>


using namespace governikus;


namespace governikus
{
template<>
int encodeAsn1Object<ECDSA_SIG>(const ECDSA_SIG* pObject, uchar** encoded)
{
	return i2d_ECDSA_SIG(i2d_const_cast(ECDSA_SIG, pObject), encoded);
}


}    // namespace governikus


Q_DECLARE_LOGGING_CATEGORY(card)


SignatureChecker::SignatureChecker(const QList<QSharedPointer<const CVCertificate>>& pCertificateChain)
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
	const EcdsaPublicKey* parentKey = &signingCert->getBody().getPublicKey();
	if (!parentKey->isComplete())
	{
		qCCritical(card) << "No elliptic curve parameters";
		return false;
	}

	for (const auto& cert : mCertificateChain)
	{
		if (!checkSignature(cert, signingCert, parentKey))
		{
			qCCritical(card) << "Certificate verification failed:" << cert->getBody().getCertificateHolderReference();
			return false;
		}

		if (const auto& certKey = cert->getBody().getPublicKey(); certKey.isComplete())
		{
			parentKey = &certKey;
		}
		signingCert = cert;
	}

	return true;
}


bool SignatureChecker::checkSignature(const QSharedPointer<const CVCertificate>& pCert, const QSharedPointer<const CVCertificate>& pSigningCert, const EcdsaPublicKey* pKey)
{
	// Some keys are "incomplete": So we need to use the parameters of the parent key and current public point.
	const QSharedPointer<EVP_PKEY> signingKey = pKey->createKey(pSigningCert->getBody().getPublicKey().getUncompressedPublicPoint());
	if (signingKey.isNull())
	{
		qCCritical(card) << "Cannot fetch signing key";
		return false;
	}

	return checkSignature(signingKey, pCert->getSignature(), pCert->getRawBody(), pSigningCert->getBody().getHashAlgorithm());
}


bool SignatureChecker::checkSignature(const QSharedPointer<EVP_PKEY>& pSigningKey, const QByteArray& pSignature, const QByteArray& pData, QCryptographicHash::Algorithm pAlgorithm)
{
	ERR_clear_error();

	if (pSigningKey.isNull())
	{
		qCCritical(card) << "Missing signing key";
		return false;
	}

	const QSharedPointer<EVP_PKEY_CTX> ctx = EcUtil::create(EVP_PKEY_CTX_new(pSigningKey.data(), nullptr));
	if (EVP_PKEY_verify_init(ctx.data()) <= 0)
	{
		qCCritical(card) << "Cannot init verify ctx";
		return false;
	}

	const auto& hash = QCryptographicHash::hash(pData, pAlgorithm);
	const auto& signature = plainToOpenSsl(pSignature);
	const int result = EVP_PKEY_verify(ctx.data(),
			reinterpret_cast<const uchar*>(signature.data()),
			static_cast<size_t>(signature.size()),
			reinterpret_cast<const uchar*>(hash.data()),
			static_cast<size_t>(hash.size()));

	if (result != 1 && result != 0)
	{
		qCCritical(card) << "Signature verification failed, an error occurred:" << getOpenSslError();
	}

	return result == 1;
}


QByteArray SignatureChecker::plainToOpenSsl(const QByteArray& pSignature)
{
	QSharedPointer<ECDSA_SIG> ecdsaSig(ECDSA_SIG_new(), [](ECDSA_SIG* pSig){ECDSA_SIG_free(pSig);});

	const auto numLen = static_cast<int>(pSignature.size()) / 2;
	const auto& randomX = pSignature.first(numLen);
	const auto& signature = pSignature.last(numLen);

	BIGNUM* r = BN_bin2bn(reinterpret_cast<const uchar*>(randomX.data()), numLen, nullptr);
	BIGNUM* s = BN_bin2bn(reinterpret_cast<const uchar*>(signature.data()), numLen, nullptr);
	ECDSA_SIG_set0(ecdsaSig.data(), r, s);

	return encodeObject<const ECDSA_SIG>(ecdsaSig.data());
}
