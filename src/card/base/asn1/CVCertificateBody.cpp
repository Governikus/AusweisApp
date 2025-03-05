/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "CVCertificateBody.h"

#include "ASN1Util.h"
#include "SecurityProtocol.h"
#include "asn1/ASN1Struct.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{

ASN1_ITEM_TEMPLATE(CertificateProfileIdentifier) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x29, CertificateProfileIdentifier, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(CertificateProfileIdentifier)

ASN1_ITEM_TEMPLATE(CAR) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x02, CAR, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(CAR)

ASN1_ITEM_TEMPLATE(CHR) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x20, CHR, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(CHR)

ASN1_ITEM_TEMPLATE(DATE_EFF) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x25, DATE, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(DATE_EFF)

ASN1_ITEM_TEMPLATE(DATE_EXPIRE) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x24, DATE, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(DATE_EXPIRE)

ASN1_ITEM_TEMPLATE(CERTIFICATEEXTENSIONS) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x05, CERTIFICATEEXTENSIONS, ASN1_ANY)
ASN1_ITEM_TEMPLATE_END(CERTIFICATEEXTENSIONS)

ASN1_SEQUENCE(certificateprofilebody_st) = {
	ASN1_SIMPLE(certificateprofilebody_st, mCertificateProfileIdentifier, CertificateProfileIdentifier),
	ASN1_SIMPLE(certificateprofilebody_st, mCertificationAuthorityReference, CAR),
	ASN1_SIMPLE(certificateprofilebody_st, mPublicKey, EcdsaPublicKey),
	ASN1_SIMPLE(certificateprofilebody_st, mCertificateHolderReference, CHR),
	ASN1_SIMPLE(certificateprofilebody_st, mChat, CHAT),
	ASN1_SIMPLE(certificateprofilebody_st, mEffectiveDate, DATE_EFF),
	ASN1_SIMPLE(certificateprofilebody_st, mExpirationDate, DATE_EXPIRE),
	ASN1_OPT(certificateprofilebody_st, mExtensions, CERTIFICATEEXTENSIONS)
}


ASN1_SEQUENCE_END(certificateprofilebody_st)


ASN1_ITEM_TEMPLATE(CVCertificateBody) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x4E, CVCertificateBody, certificateprofilebody_st)
ASN1_ITEM_TEMPLATE_END(CVCertificateBody)


IMPLEMENT_ASN1_FUNCTIONS(CVCertificateBody)
IMPLEMENT_ASN1_OBJECT(CVCertificateBody)
} // namespace governikus


QSharedPointer<CVCertificateBody> CVCertificateBody::decode(const QByteArray& pBytes)
{
	return decodeObject<CVCertificateBody>(pBytes);
}


QByteArray CVCertificateBody::encode()
{
	return encodeObject(this);
}


QByteArray CVCertificateBody::getCertificateProfileIdentifier() const
{
	return Asn1OctetStringUtil::getValue(mCertificateProfileIdentifier);
}


QByteArray CVCertificateBody::getCertificationAuthorityReference() const
{
	return Asn1OctetStringUtil::getValue(mCertificationAuthorityReference);
}


const EcdsaPublicKey& CVCertificateBody::getPublicKey() const
{
	Q_ASSERT(mPublicKey != nullptr);
	return *mPublicKey;
}


const CHAT& CVCertificateBody::getCHAT() const
{
	Q_ASSERT(mChat != nullptr);
	return *mChat;
}


void CVCertificateBody::setCertificateExpirationDate(QDate date)
{
	QByteArray array = Asn1BCDDateUtil::convertFromQDateToUnpackedBCD(date);
	Asn1OctetStringUtil::setValue(array, mExpirationDate);
}


QDate CVCertificateBody::getCertificateExpirationDate() const
{
	return Asn1BCDDateUtil::convertFromUnpackedBCDToQDate(mExpirationDate);
}


void CVCertificateBody::setCertificateEffectiveDate(QDate date)
{
	QByteArray array = Asn1BCDDateUtil::convertFromQDateToUnpackedBCD(date);
	Asn1OctetStringUtil::setValue(array, mEffectiveDate);
}


QDate CVCertificateBody::getCertificateEffectiveDate() const
{
	return Asn1BCDDateUtil::convertFromUnpackedBCDToQDate(mEffectiveDate);
}


QByteArray CVCertificateBody::getCertificateHolderReference() const
{
	return Asn1OctetStringUtil::getValue(mCertificateHolderReference);
}


QCryptographicHash::Algorithm CVCertificateBody::getHashAlgorithm() const
{
	return getPublicKey().getSecurityProtocol().getHashAlgorithm();
}


QByteArray CVCertificateBody::getExtension(const Oid& pOid) const
{
	if (mExtensions)
	{
		for (int i = 0; i < sk_ASN1_TYPE_num(mExtensions); i++)
		{
			const ASN1Struct extension(Asn1TypeUtil::encode(sk_ASN1_TYPE_value(mExtensions, i)));
			if (Oid(extension.getData(V_ASN1_UNIVERSAL, ASN1Struct::UNI_OBJECT_IDENTIFIER)) == pOid)
			{
				// Currently we only return the first simple ASN.1 value found in the structure. The
				// only known case where more information is given is ID_SECTOR_PS (BSI TR-03110-3
				// C.3.2.2). But this information is not used/requested by the eID-Client.
				return extension.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::CERTIFICATE_EXTENSION_CONTENT_0);
			}
		}
	}

	return QByteArray();
}
