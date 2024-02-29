/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/SignatureChecker.h"

#include "ASN1TemplateUtil.h"
#include "pace/ec/EcUtil.h"

#include <QLoggingCategory>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <openssl/evp.h>

using namespace governikus;


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


bool SignatureChecker::checkSignature(const QSharedPointer<const CVCertificate>& pCert, const QSharedPointer<const CVCertificate>& pSigningCert, const EcdsaPublicKey* pKey) const
{
	ERR_clear_error();

	// Some keys are "incomplete": So we need to use the parameters of the parent key and current public point.
	const QSharedPointer<EVP_PKEY> signingKey = pKey->createKey(pSigningCert->getBody().getPublicKey().getUncompressedPublicPoint());
	if (signingKey.isNull())
	{
		qCCritical(card) << "Cannot fetch signing key";
		return false;
	}

	const QSharedPointer<EVP_PKEY_CTX> ctx = EcUtil::create(EVP_PKEY_CTX_new(signingKey.data(), nullptr));
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
		qCCritical(card) << "Signature verification failed, an error occurred:" << getOpenSslError();
	}

	return result == 1;
}
