/*!
 * \brief Implementation of Card Verifiable Certificate, CVC.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once


#include "CVCertificateBody.h"


#include <QByteArrayList>
#include <QDebug>
#include <QString>

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


typedef struct cvcertificate_st
{
	CVCertificateBody* mBody;
	SIGNATURE* mSignature;
	QSharedPointer<ECDSA_SIG> mEcdsaSignature;

	static QVector<QSharedPointer<cvcertificate_st> > fromHex(const QByteArrayList& pHexByteList);
	static QSharedPointer<cvcertificate_st> fromHex(const QByteArray& pHexBytes);
	QByteArray encode() const;

	bool operator==(const cvcertificate_st& other) const;

	const CVCertificateBody& getBody() const;
	QByteArray getRawBody() const;
	QSharedPointer<const ECDSA_SIG> getEcdsaSignature() const;
	QByteArray getRawSignature() const;


	bool isValidOn(const QDateTime& pValidationDate) const;
	bool isIssuedBy(const cvcertificate_st& pIssuer) const;

	static int decodeCallback(int pOperation, ASN1_VALUE** pVal, const ASN1_ITEM* pIt, void* pExarg);

} CVCertificate;


DECLARE_ASN1_FUNCTIONS(CVCertificate)
DECLARE_ASN1_OBJECT(CVCertificate)


} /* namespace governikus */


bool operator==(const QSharedPointer<governikus::CVCertificate>& pCvc1, const QSharedPointer<governikus::CVCertificate>& pCvc2);
bool operator==(const QSharedPointer<governikus::CVCertificate>& pCvc1, const QSharedPointer<const governikus::CVCertificate>& pCvc2);
bool operator==(const QSharedPointer<const governikus::CVCertificate>& pCvc1, const QSharedPointer<governikus::CVCertificate>& pCvc2);
bool operator==(const QSharedPointer<const governikus::CVCertificate>& pCvc1, const QSharedPointer<const governikus::CVCertificate>& pCvc2);


QDebug operator<<(QDebug pDbg, const governikus::CVCertificate& pCvc);
QDebug operator<<(QDebug pDbg, const QSharedPointer<const governikus::CVCertificate>& pCvc);
QDebug operator<<(QDebug pDbg, QSharedPointer<governikus::CVCertificate>& pCvc);
QDebug operator<<(QDebug pDbg, const QVector<QSharedPointer<governikus::CVCertificate> >& pCvcs);
