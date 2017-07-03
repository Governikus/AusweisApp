/*!
 * EFCardSecurity.h
 *
 * \brief Implementation of EFCardSecurity
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */


#pragma once


#include "SecurityInfos.h"

#include <openssl/cms.h>

#include <QByteArray>


namespace governikus
{


/*!
 * EF.CardSecurity is defined in TR-03110-3 as ContentInfo with contentType id-signedData,
 * where the SignedData has eContentType id-SecurityObject.
 *
 *
 * id-signedData OBJECT IDENTIFIER ::= { iso(1) member-body(2)
 *      us(840) rsadsi(113549) pkcs(1) pkcs7(7) 2 }
 *
 *
 * id-SecurityObject OBJECT IDENTIFIER ::= {
 *      bsi-de applications(3) eID(2) 1
 * }
 *
 *
 * ContentInfo ::= SEQUENCE {
 *      contentType ContentType,
 *      content [0] EXPLICIT ANY DEFINED BY contentType
 * }
 *
 *
 * ContentType ::= OBJECT IDENTIFIER
 *
 *
 * SignedData ::= SEQUENCE {
 *      version CMSVersion,
 *      digestAlgorithms DigestAlgorithmIdentifiers,
 *      encapContentInfo EncapsulatedContentInfo,
 *      certificates [0] IMPLICIT CertificateSet OPTIONAL,
 *      crls [1] IMPLICIT RevocationInfoChoices OPTIONAL,
 *      signerInfos SignerInfos
 * }
 *
 *
 * DigestAlgorithmIdentifiers ::= SET OF DigestAlgorithmIdentifier
 *
 *
 * EncapsulatedContentInfo ::= SEQUENCE {
 *      eContentType ContentType,
 *      eContent [0] EXPLICIT OCTET STRING OPTIONAL
 * }
 *
 *
 * SignerInfos ::= SET OF SignerInfo
 *
 *
 * SignerInfo ::= SEQUENCE {
 *      version CMSVersion,
 *      sid SignerIdentifier,
 *      digestAlgorithm DigestAlgorithmIdentifier,
 *      signedAttrs [0] IMPLICIT SignedAttributes OPTIONAL,
 *      signatureAlgoritm SignatureAlgorithmIdentifier,
 *      signature SignatureValue,
 *      unsignedAttrs [1] IMPLICIT UnsignedAttributes OPTIONAL
 * }
 *
 *
 * SignerIdentifier ::= CHOICE {
 *      issuerAndSerialNumber IssuerAndSerialNumber,
 *      subjectKeyIdentifier [0] SubjectKeyIdentifier
 * }
 *
 *
 * SignatureValue ::= OCTET STRING
 */
class EFCardSecurity
{
	QSharedPointer<SecurityInfos> mSecurityInfos;

	EFCardSecurity(QSharedPointer<SecurityInfos> pSecurityInfos);
	Q_DISABLE_COPY(EFCardSecurity)

	public:
		static QSharedPointer<EFCardSecurity> fromHex(const QByteArray& pHexString);
		static QSharedPointer<EFCardSecurity> decode(const QByteArray& pBytes);

		template<typename T> QVector<QSharedPointer<T> > getSecurityInfos() const
		{
			Q_ASSERT(mSecurityInfos != nullptr);
			return mSecurityInfos->getSecurityInfos<T>();
		}


};

template<>
CMS_ContentInfo* decodeAsn1Object<CMS_ContentInfo>(CMS_ContentInfo** pObject, const unsigned char** pData, long pDataLen);


template<>
void freeAsn1Object<CMS_ContentInfo>(CMS_ContentInfo* pObject);

}  // namespace governikus
