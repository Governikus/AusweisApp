/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QVector>

#include "asn1/CVCertificate.h"


namespace governikus
{

class SignatureChecker
{
	private:
		const QVector<QSharedPointer<const CVCertificate>> mCertificateChain;

		bool checkSignature(const QSharedPointer<const CVCertificate>& pCert, const QSharedPointer<const CVCertificate>& pSigningCert, const EcdsaPublicKey* pKey) const;

	public:
		explicit SignatureChecker(const QVector<QSharedPointer<const CVCertificate>>& pCertificateChain);
		~SignatureChecker() = default;

		[[nodiscard]] bool check() const;
};

} // namespace governikus
