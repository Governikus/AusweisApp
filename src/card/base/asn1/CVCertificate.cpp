/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "CVCertificate.h"
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
	Q_UNUSED(pIt);
	Q_UNUSED(pExarg);
	if (pOperation == ASN1_OP_D2I_POST)
	{
		if (auto cvc = reinterpret_cast<cvcertificate_st*>(*pVal))
		{
			cvc->mEcdsaSignature = EcUtil::create(ECDSA_SIG_new());
			QByteArray sigValue = Asn1OctetStringUtil::getValue(cvc->mSignature);

			const unsigned char* sig = reinterpret_cast<const unsigned char*>(sigValue.data());
			int siglen = sigValue.size();

			BIGNUM* r = BN_bin2bn(sig, siglen / 2, 0);
			BIGNUM* s = BN_bin2bn(sig + (siglen / 2), siglen / 2, 0);

#if OPENSSL_VERSION_NUMBER < 0x10100000L
			cvc->mEcdsaSignature.data()->r = r;
			cvc->mEcdsaSignature.data()->s = s;
#else
			ECDSA_SIG_set0(cvc->mEcdsaSignature.data(), r, s);
#endif

		}
	}
	return CB_SUCCESS;
}


QVector<QSharedPointer<const CVCertificate> > CVCertificate::fromHex(const QByteArrayList& pHexByteList)
{
	QVector<QSharedPointer<const CVCertificate> > cvcs;
	for (const QByteArray& hexBytes : pHexByteList)
	{
		if (auto cvc = CVCertificate::fromHex(hexBytes))
		{
			cvcs += cvc;
		}
	}
	return cvcs;
}


QSharedPointer<const CVCertificate> CVCertificate::fromHex(const QByteArray& pHexBytes)
{
	return decodeObject<CVCertificate>(QByteArray::fromHex(pHexBytes));

}


QByteArray CVCertificate::encode() const
{
	return encodeObject(const_cast<CVCertificate*>(this));
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


QSharedPointer<const ECDSA_SIG> CVCertificate::getEcdsaSignature() const
{
	return mEcdsaSignature;
}


QByteArray CVCertificate::getRawSignature() const
{
	return encodeObject(const_cast<SIGNATURE*>(mSignature));
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
				   << "," << pCvc.getBody().getCertificateExpirationDate().toString(Qt::ISODate) << "])";
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


QDebug operator<<(QDebug pDbg, QSharedPointer<governikus::CVCertificate>& pCvc)
{
	const QSharedPointer<const governikus::CVCertificate> constPtr(pCvc);
	pDbg << constPtr;

	return pDbg;
}


QDebug operator<<(QDebug pDbg, const QVector<QSharedPointer<governikus::CVCertificate> >& pCvcs)
{
	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << "QVector(";
	for (auto cvc : pCvcs)
	{
		pDbg.nospace() << cvc << ",";
	}
	pDbg.nospace() << ")";
	return pDbg;
}
