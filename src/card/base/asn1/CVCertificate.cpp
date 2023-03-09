/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "CVCertificate.h"

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "pace/ec/EcUtil.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


namespace governikus
{

ASN1_ITEM_TEMPLATE(SIGNATURE) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x37, SIGNATURE, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(SIGNATURE)


IMPLEMENT_ASN1_FUNCTIONS(SIGNATURE)
IMPLEMENT_ASN1_OBJECT(SIGNATURE)


ASN1_SEQUENCE_cb(cvcertificate_st, CVCertificate::decodeCallback) = {
	ASN1_SIMPLE(cvcertificate_st, mBody, CVCertificateBody),
	ASN1_SIMPLE(cvcertificate_st, mSignature, SIGNATURE)
}


ASN1_SEQUENCE_END_cb(cvcertificate_st, cvcertificate_st)

ASN1_ITEM_TEMPLATE(CVCertificate) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x21, CVCertificate, cvcertificate_st)
ASN1_ITEM_TEMPLATE_END(CVCertificate)

IMPLEMENT_ASN1_FUNCTIONS(CVCertificate)
IMPLEMENT_ASN1_OBJECT(CVCertificate)


}  // namespace governikus


int CVCertificate::decodeCallback(int pOperation, ASN1_VALUE** pVal, const ASN1_ITEM* pIt, void* pExarg)
{
	Q_UNUSED(pIt)
	Q_UNUSED(pExarg)
	if (pOperation == ASN1_OP_D2I_POST)
	{
		if (auto cvc = reinterpret_cast<cvcertificate_st*>(*pVal))
		{
			cvc->mEcdsaSignature = ECDSA_SIG_new();
			QByteArray sigValue = Asn1OctetStringUtil::getValue(cvc->mSignature);

			const auto* const sig = reinterpret_cast<const uchar*>(sigValue.data());
			int siglen = sigValue.size();

			BIGNUM* r = BN_bin2bn(sig, siglen / 2, nullptr);
			BIGNUM* s = BN_bin2bn(sig + (siglen / 2), siglen / 2, nullptr);

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
			cvc->mEcdsaSignature->r = r;
			cvc->mEcdsaSignature->s = s;
#else
			ECDSA_SIG_set0(cvc->mEcdsaSignature, r, s);
#endif
		}
	}
	else if (pOperation == ASN1_OP_FREE_POST)
	{
		if (auto cvc = reinterpret_cast<cvcertificate_st*>(*pVal))
		{
			ECDSA_SIG_free(cvc->mEcdsaSignature);
		}
	}

	return CB_SUCCESS;
}


QVector<QSharedPointer<const CVCertificate>> CVCertificate::fromRaw(const QByteArrayList& pByteList)
{
	QVector<QSharedPointer<const CVCertificate>> cvcs;
	for (const auto& data : pByteList)
	{
		if (const auto& cvc = CVCertificate::fromRaw(data))
		{
			cvcs += cvc;
		}
	}
	return cvcs;
}


QSharedPointer<const CVCertificate> CVCertificate::fromRaw(const QByteArray& pBytes)
{
	return decodeObject<CVCertificate>(pBytes);
}


#ifndef QT_NO_DEBUG
QSharedPointer<const CVCertificate> CVCertificate::fromHex(const QByteArray& pBytes)
{
	return fromRaw(QByteArray::fromHex(pBytes));
}


#endif


QByteArray CVCertificate::encode() const
{
	return encodeObject(this);
}


const CVCertificateBody& CVCertificate::getBody() const
{
	Q_ASSERT(mBody != nullptr);
	return *mBody;
}


QByteArray CVCertificate::getRawBody() const
{
	Q_ASSERT(mBody != nullptr);
	return mBody->encode();
}


const ECDSA_SIG* CVCertificate::getEcdsaSignature() const
{
	return mEcdsaSignature;
}


QByteArray CVCertificate::getRawSignature() const
{
	return encodeObject(mSignature);
}


QByteArray CVCertificate::getDerSignature() const
{
	uchar* signature = nullptr;
	const int size = i2d_ECDSA_SIG(mEcdsaSignature, &signature);
	const QByteArray result(reinterpret_cast<char*>(signature), size);
	OPENSSL_free(signature);
	return result;
}


bool CVCertificate::isValidOn(const QDateTime& pValidationDate) const
{
	QDate validationDateGMT = pValidationDate.toUTC().date();
	return(validationDateGMT <= getBody().getCertificateExpirationDate() && validationDateGMT >= getBody().getCertificateEffectiveDate());
}


bool CVCertificate::isIssuedBy(const CVCertificate& pIssuer) const
{
	return getBody().getCertificationAuthorityReference() == pIssuer.getBody().getCertificateHolderReference();
}


QDebug operator <<(QDebug pDbg, const governikus::CVCertificate& pCvc)
{
	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << "CVC(type=" << pCvc.getBody().getCHAT().getAccessRole()
				   << ", car=" << pCvc.getBody().getCertificationAuthorityReference()
				   << ", chr=" << pCvc.getBody().getCertificateHolderReference()
				   << ", valid=[" << pCvc.getBody().getCertificateEffectiveDate().toString(Qt::ISODate)
				   << "," << pCvc.getBody().getCertificateExpirationDate().toString(Qt::ISODate) << "]="
				   << pCvc.isValidOn(QDateTime::currentDateTime()) << ")";
	return pDbg;
}


QDebug operator<<(QDebug pDbg, const QSharedPointer<const governikus::CVCertificate>& pCvc)
{
	if (pCvc == nullptr)
	{
		QDebugStateSaver saver(pDbg);
		pDbg.nospace() << "false";
		return pDbg;
	}
	else
	{
		return operator<<(pDbg, *pCvc);
	}
}


QDebug operator<<(QDebug pDbg, const QSharedPointer<governikus::CVCertificate>& pCvc)
{
	pDbg << QSharedPointer<const governikus::CVCertificate>(pCvc);
	return pDbg;
}


QDebug operator<<(QDebug pDbg, const QVector<QSharedPointer<governikus::CVCertificate>>& pCvcs)
{
	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << "QVector(";
	for (const auto& cvc : pCvcs)
	{
		pDbg.nospace() << cvc << ",";
	}
	pDbg.nospace() << ")";
	return pDbg;
}
