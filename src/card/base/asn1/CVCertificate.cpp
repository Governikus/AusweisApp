/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
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
			BN_bin2bn(sig, siglen / 2, cvc->mEcdsaSignature.data()->r);
			BN_bin2bn(sig + (siglen / 2), siglen / 2, cvc->mEcdsaSignature.data()->s);
		}
	}
	return CB_SUCCESS;
}


QVector<QSharedPointer<CVCertificate> > CVCertificate::fromHex(const QByteArrayList& pHexByteList)
{
	QVector<QSharedPointer<CVCertificate> > cvcs;
	for (const QByteArray& hexBytes : pHexByteList)
	{
		if (auto cvc = CVCertificate::fromHex(hexBytes))
		{
			cvcs += cvc;
		}
	}
	return cvcs;
}


QSharedPointer<CVCertificate> CVCertificate::fromHex(const QByteArray& pHexBytes)
{
	return decodeObject<CVCertificate>(QByteArray::fromHex(pHexBytes));

}


QByteArray CVCertificate::encode() const
{
	return encodeObject(const_cast<CVCertificate*>(this));
}


bool CVCertificate::operator ==(const cvcertificate_st& other) const
{
	return this->getRawBody() == other.getRawBody() && this->getRawSignature() == other.getRawSignature();
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
	return this->getBody().getCertificationAuthorityReference() == pIssuer.getBody().getCertificateHolderReference();
}


bool operator==(const QSharedPointer<governikus::CVCertificate>& pCvc1, const QSharedPointer<governikus::CVCertificate>& pCvc2)
{
	return QSharedPointer<const governikus::CVCertificate>(pCvc1) == QSharedPointer<const governikus::CVCertificate>(pCvc2);
}


bool operator==(const QSharedPointer<governikus::CVCertificate>& pCvc1, const QSharedPointer<const governikus::CVCertificate>& pCvc2)
{
	return QSharedPointer<const governikus::CVCertificate>(pCvc1) == pCvc2;
}


bool operator==(const QSharedPointer<const governikus::CVCertificate>& pCvc1, const QSharedPointer<governikus::CVCertificate>& pCvc2)
{
	return pCvc1 == QSharedPointer<const governikus::CVCertificate>(pCvc2);
}


bool operator==(const QSharedPointer<const governikus::CVCertificate>& pCvc1, const QSharedPointer<const governikus::CVCertificate>& pCvc2)
{
	if ((pCvc1.isNull() && !pCvc2.isNull()) || (!pCvc1.isNull() && pCvc2.isNull()))
	{
		return false;
	}
	if (pCvc1.isNull() && pCvc2.isNull())
	{
		return true;
	}
	return *pCvc1 == *pCvc2;
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
