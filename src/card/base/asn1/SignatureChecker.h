/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QVector>

#include "asn1/CVCertificate.h"


namespace governikus
{

class SignatureChecker
{
	private:
		const QVector<QSharedPointer<CVCertificate> > mCertificateChain;

		bool checkSignature(const QSharedPointer<const CVCertificate>& pCert, const QSharedPointer<const CVCertificate>& pSigningCert, const EC_KEY* pKey);

	public:
		SignatureChecker(const QVector<QSharedPointer<CVCertificate> >& pCertificateChain);
		~SignatureChecker() = default;

		bool check();
};

} /* namespace governikus */
