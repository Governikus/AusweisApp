/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/CVCertificate.h"

#include <QList>

namespace governikus
{

class SignatureChecker
{
	private:
		const QList<QSharedPointer<const CVCertificate>> mCertificateChain;

		bool checkSignature(const QSharedPointer<const CVCertificate>& pCert, const QSharedPointer<const CVCertificate>& pSigningCert, const EcdsaPublicKey* pKey) const;

	public:
		explicit SignatureChecker(const QList<QSharedPointer<const CVCertificate>>& pCertificateChain);
		~SignatureChecker() = default;

		[[nodiscard]] bool check() const;
};

} // namespace governikus
