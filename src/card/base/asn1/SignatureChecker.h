/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/CVCertificate.h"

#include <QByteArray>
#include <QList>
#include <QSharedPointer>


namespace governikus
{

class SignatureChecker
{
	private:
		const QList<QSharedPointer<const CVCertificate>> mCertificateChain;

	public:
		explicit SignatureChecker(const QList<QSharedPointer<const CVCertificate>>& pCertificateChain);
		~SignatureChecker() = default;

		[[nodiscard]] bool check() const;
		static bool checkSignature(const QSharedPointer<const CVCertificate>& pCert, const QSharedPointer<const CVCertificate>& pSigningCert, const EcdsaPublicKey* pKey);
		static bool checkSignature(const QSharedPointer<EVP_PKEY>& pSigningKey, const QByteArray& pSignature, const QByteArray& pData, QCryptographicHash::Algorithm pAlgorithm);

	private:
		static QByteArray plainToOpenSsl(const QByteArray& pSignature);
};

} // namespace governikus
