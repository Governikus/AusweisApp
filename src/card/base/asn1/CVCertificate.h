/*!
 * \brief Implementation of Card Verifiable Certificate, CVC.
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "CVCertificateBody.h"


#include <QByteArrayList>
#include <QDebug>

#include <openssl/ecdsa.h>


namespace governikus
{

/*!
 * According to
 *    - TR-03110-3, chapter C.1 and
 *    - TR-03110-3, chapter D.2 and
 *
 * CVCertificate ::= APPLICATION [0x21] IMPLICIT SEQUENCE {
 *      body CVCertificateBody,
 *      signature SIGNATURE
 * }
 *
 * SIGNATURE ::= APPLICATION [0x37] IMPLICIT OCTET STRING
 *
 */


struct SIGNATURE
	: public ASN1_OCTET_STRING
{
	// we use inheritance, because the structure ValidityDate is already a typedef of ASN1_OCTET_STRING
	// and we cannot have two template specializations for the same type.
};


using CVCertificate = struct cvcertificate_st
{
	CVCertificateBody* mBody;
	SIGNATURE* mSignature;
	ECDSA_SIG* mEcdsaSignature;

	static QVector<QSharedPointer<const cvcertificate_st>> fromHex(const QByteArrayList& pHexByteList);
	static QSharedPointer<const cvcertificate_st> fromHex(const QByteArray& pHexBytes);
	[[nodiscard]] QByteArray encode() const;

	[[nodiscard]] const CVCertificateBody& getBody() const;
	[[nodiscard]] QByteArray getRawBody() const;
	[[nodiscard]] const ECDSA_SIG* getEcdsaSignature() const;
	[[nodiscard]] QByteArray getRawSignature() const;
	[[nodiscard]] QByteArray getDerSignature() const;

	[[nodiscard]] bool isValidOn(const QDateTime& pValidationDate) const;
	[[nodiscard]] bool isIssuedBy(const cvcertificate_st& pIssuer) const;

	static int decodeCallback(int pOperation, ASN1_VALUE** pVal, const ASN1_ITEM* pIt, void* pExarg);
};


DECLARE_ASN1_FUNCTIONS(CVCertificate)
DECLARE_ASN1_OBJECT(CVCertificate)


inline bool operator==(const CVCertificate& pLeft, const CVCertificate& pRight)
{
	return pLeft.getRawBody() == pRight.getRawBody() && pLeft.getRawSignature() == pRight.getRawSignature();
}


inline bool operator!=(const CVCertificate& pLeft, const CVCertificate& pRight)
{
	return !(pLeft == pRight);
}


} // namespace governikus

QDebug operator<<(QDebug pDbg, const governikus::CVCertificate& pCvc);
QDebug operator<<(QDebug pDbg, const QSharedPointer<const governikus::CVCertificate>& pCvc);
QDebug operator<<(QDebug pDbg, QSharedPointer<governikus::CVCertificate>& pCvc);
QDebug operator<<(QDebug pDbg, const QVector<QSharedPointer<governikus::CVCertificate>>& pCvcs);
