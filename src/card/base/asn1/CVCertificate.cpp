/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#include "CVCertificate.h"

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"


Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


namespace governikus
{

ASN1_ITEM_TEMPLATE(SIGNATURE) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x37, SIGNATURE, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(SIGNATURE)


IMPLEMENT_ASN1_FUNCTIONS(SIGNATURE)
IMPLEMENT_ASN1_OBJECT(SIGNATURE)


ASN1_SEQUENCE(cvcertificate_st) = {
	ASN1_SIMPLE(cvcertificate_st, mBody, CVCertificateBody),
	ASN1_SIMPLE(cvcertificate_st, mSignature, SIGNATURE)
}


ASN1_SEQUENCE_END(cvcertificate_st)

ASN1_ITEM_TEMPLATE(CVCertificate) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x21, CVCertificate, cvcertificate_st)
ASN1_ITEM_TEMPLATE_END(CVCertificate)

IMPLEMENT_ASN1_FUNCTIONS(CVCertificate)
IMPLEMENT_ASN1_OBJECT(CVCertificate)


}  // namespace governikus


QList<QSharedPointer<const CVCertificate>> CVCertificate::fromRaw(const QByteArrayList& pByteList)
{
	QList<QSharedPointer<const CVCertificate>> cvcs;
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


QByteArray CVCertificate::getSignature() const
{
	return Asn1OctetStringUtil::getValue(mSignature);
}


QByteArray CVCertificate::getRawSignature() const
{
	return encodeObject(mSignature);
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


bool CVCertificate::operator==(const cvcertificate_st& pOther) const
{
	return getRawBody() == pOther.getRawBody() && getRawSignature() == pOther.getRawSignature();
}


bool CVCertificate::operator!=(const cvcertificate_st& pOther) const
{
	return !(*this == pOther);
}


QDebug operator <<(QDebug pDbg, const governikus::CVCertificate& pCvc)
{
	QDebugStateSaver saver(pDbg);
	pDbg.nospace().noquote() << pCvc.getBody().getCHAT().getAccessRole() << "("
							 << pCvc.getBody().getCertificateHolderReference().trimmed()
							 << ", authority=" << pCvc.getBody().getCertificationAuthorityReference().trimmed()
							 << ", " << pCvc.getBody().getCertificateEffectiveDate().toString(Qt::ISODate)
							 << " - " << pCvc.getBody().getCertificateExpirationDate().toString(Qt::ISODate)
							 << ", " << (pCvc.isValidOn(QDateTime::currentDateTime()) ? "valid" : "invalid")
							 << ")";
	return pDbg;
}


QDebug operator<<(QDebug pDbg, const QSharedPointer<const governikus::CVCertificate>& pCvc)
{
	if (pCvc == nullptr)
	{
		QDebugStateSaver saver(pDbg);
		pDbg.nospace() << "0x0";
		return pDbg;
	}
	else
	{
		return operator<<(pDbg, *pCvc);
	}
}


QDebug operator<<(QDebug pDbg, const QList<QSharedPointer<const governikus::CVCertificate>>& pCvcs)
{
	QByteArrayList names;
	for (const auto& cvc : pCvcs)
	{
		QByteArray holder = cvc->getBody().getCertificateHolderReference().trimmed();
		if (cvc->isIssuedBy(*cvc))
		{
			holder.prepend("(Self Signed) ");
		}
		names << holder;
	}

	QDebugStateSaver saver(pDbg);
	pDbg.noquote().nospace() << "[" << names.join(", ") << "]";
	return pDbg;
}
