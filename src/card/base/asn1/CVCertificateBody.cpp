/*
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "CVCertificateBody.h"

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "KnownOIDs.h"

#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{


ASN1_ITEM_TEMPLATE(CertificateProfileIdentifier) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x29, CertificateProfileIdentifier, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(CertificateProfileIdentifier)


ASN1_SEQUENCE(CERTIFICATEEXTENSION_st) = {

	ASN1_SIMPLE(CERTIFICATEEXTENSION_st, mOid, ASN1_OBJECT),
	ASN1_IMP_OPT(CERTIFICATEEXTENSION_st, mObject1, ASN1_OCTET_STRING, 0x00),
	ASN1_IMP_OPT(CERTIFICATEEXTENSION_st, mObject2, ASN1_OCTET_STRING, 0x01),
	ASN1_IMP_OPT(CERTIFICATEEXTENSION_st, mObject3, ASN1_OCTET_STRING, 0x02),
	ASN1_IMP_OPT(CERTIFICATEEXTENSION_st, mObject4, ASN1_OCTET_STRING, 0x03),
	ASN1_IMP_OPT(CERTIFICATEEXTENSION_st, mObject5, ASN1_OCTET_STRING, 0x04),
	ASN1_IMP_OPT(CERTIFICATEEXTENSION_st, mObject6, ASN1_OCTET_STRING, 0x05),
	ASN1_IMP_OPT(CERTIFICATEEXTENSION_st, mObject7, ASN1_OCTET_STRING, 0x06),
	ASN1_IMP_OPT(CERTIFICATEEXTENSION_st, mObject8, ASN1_OCTET_STRING, 0x07)
}


ASN1_SEQUENCE_END(CERTIFICATEEXTENSION_st)

IMPLEMENT_ASN1_FUNCTIONS(CERTIFICATEEXTENSION)

ASN1_ITEM_TEMPLATE(CERTIFICATEEXTENSION) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x13, CERTIFICATEEXTENSION, CERTIFICATEEXTENSION_st)
ASN1_ITEM_TEMPLATE_END(CERTIFICATEEXTENSION)


ASN1_ITEM_TEMPLATE(CERTIFICATEEXTENSIONS) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x05, CERTIFICATEEXTENSIONS, CERTIFICATEEXTENSION)
ASN1_ITEM_TEMPLATE_END(CERTIFICATEEXTENSIONS)


ASN1_ITEM_TEMPLATE(CHR) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x20, CHR, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(CHR)


ASN1_ITEM_TEMPLATE(CAR) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x02, CAR, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(CAR)


ASN1_ITEM_TEMPLATE(DATE_EXPIRE) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x24, DATE, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(DATE_EXPIRE)


ASN1_ITEM_TEMPLATE(DATE_EFF) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x25, DATE, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(DATE_EFF)


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


#if OPENSSL_VERSION_NUMBER < 0x10100000L
	#define sk_CERTIFICATEEXTENSION_num(data) SKM_sk_num(CERTIFICATEEXTENSION, data)
	#define sk_CERTIFICATEEXTENSION_value(data, i) SKM_sk_value(CERTIFICATEEXTENSION, data, i)
#endif

} /* namespace governikus */


QSharedPointer<CVCertificateBody> CVCertificateBody::fromHex(const QString& pHexValue)
{
	return decode(QByteArray::fromHex(pHexValue.toLatin1()));
}


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
	const auto oid = getPublicKey().getPublicKeyOid();
	if (oid == KnownOIDs::id_ta::ECDSA_SHA_1)
	{
		return QCryptographicHash::Sha1;
	}
	if (oid == KnownOIDs::id_ta::ECDSA_SHA_224)
	{
		return QCryptographicHash::Sha224;
	}
	if (oid == KnownOIDs::id_ta::ECDSA_SHA_256)
	{
		return QCryptographicHash::Sha256;
	}
	if (oid == KnownOIDs::id_ta::ECDSA_SHA_384)
	{
		return QCryptographicHash::Sha384;
	}
	if (oid == KnownOIDs::id_ta::ECDSA_SHA_512)
	{
		return QCryptographicHash::Sha512;
	}

	qCCritical(card) << "unknown OID" << oid;
	return QCryptographicHash::Sha256;
}


QMap<QByteArray, QByteArray> CVCertificateBody::getExtensions() const
{
	QMap<QByteArray, QByteArray> ext;

	if (mExtensions != nullptr)
	{
		for (int i = 0; i < sk_CERTIFICATEEXTENSION_num(mExtensions); i++)
		{
			CERTIFICATEEXTENSION* extension = sk_CERTIFICATEEXTENSION_value(mExtensions, i);
			ext.insert(Asn1ObjectUtil::convertTo(extension->mOid), Asn1OctetStringUtil::getValue(extension->mObject1));
		}
	}

	return ext;
}
