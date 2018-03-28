/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "AuthenticatedAuxiliaryData.h"

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{

using CommunityID = ASN1_OCTET_STRING;
DECLARE_ASN1_FUNCTIONS(CommunityID)

using ValidityDate = ASN1_OCTET_STRING;
DECLARE_ASN1_FUNCTIONS(ValidityDate)

using AgeVerificationDate = ASN1_STRING;
DECLARE_ASN1_FUNCTIONS(AgeVerificationDate)

DECLARE_ASN1_OBJECT(ValidityDate)

/*!
 * This defines the AuxDataTemplate object as SEQUENCE without the special tag.
 */
ASN1_SEQUENCE(auxdatatemplate_st) = {
	ASN1_SIMPLE(auxdatatemplate_st, mAuxId, ASN1_OBJECT),
	ASN1_SIMPLE(auxdatatemplate_st, mExtInfo, ASN1_ANY),
}


ASN1_SEQUENCE_END(auxdatatemplate_st)


/*!
 * This defines the AuxDataTemplate object with the special tag 0x13.
 */
ASN1_ITEM_TEMPLATE(AuxDataTemplate) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x13, AuxDataTemplate, auxdatatemplate_st)
ASN1_ITEM_TEMPLATE_END(AuxDataTemplate)


/*!
 * This defines the AuthenticatedAuxiliaryDataInternal object with the special tag 0x07.
 */
ASN1_ITEM_TEMPLATE(AuthenticatedAuxiliaryDataInternal) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_SET_OF | ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x07, AuthenticatedAuxiliaryDataInternal, AuxDataTemplate)
ASN1_ITEM_TEMPLATE_END(AuthenticatedAuxiliaryDataInternal)


IMPLEMENT_ASN1_FUNCTIONS(AuthenticatedAuxiliaryDataInternal)
IMPLEMENT_ASN1_OBJECT(AuthenticatedAuxiliaryDataInternal)


/*!
 * This defines the CommunityID with special tag 0x13
 */
ASN1_ITEM_TEMPLATE(CommunityID) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x13, CommunityID, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(CommunityID)


IMPLEMENT_ASN1_FUNCTIONS(CommunityID)


/*!
 * This defines the ValidityDate with special tag 0x13
 */
ASN1_ITEM_TEMPLATE(ValidityDate) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x13, ValidityDate, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(ValidityDate)


IMPLEMENT_ASN1_FUNCTIONS(ValidityDate)
IMPLEMENT_ASN1_OBJECT(ValidityDate)


/*!
 * This defines the AgeVerificationDate with special tag 0x13
 */
ASN1_ITEM_TEMPLATE(AgeVerificationDate) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x13, AgeVerificationDate, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(AgeVerificationDate)


IMPLEMENT_ASN1_FUNCTIONS(AgeVerificationDate)

DECLARE_ASN1_FUNCTIONS(AuthenticatedAuxiliaryDataInternal)
DECLARE_ASN1_OBJECT(AuthenticatedAuxiliaryDataInternal)


#if OPENSSL_VERSION_NUMBER < 0x10100000L
	#define sk_AuxDataTemplate_num(data) SKM_sk_num(AuxDataTemplate, data)
	#define sk_AuxDataTemplate_value(data, i) SKM_sk_value(AuxDataTemplate, data, i)
#endif

} /* namespace governikus */


AuthenticatedAuxiliaryData::AuthenticatedAuxiliaryData(const QSharedPointer<AuthenticatedAuxiliaryDataInternal>& pData)
	: mData(pData)
{
	Q_ASSERT(mData);
}


QSharedPointer<AuthenticatedAuxiliaryData> AuthenticatedAuxiliaryData::fromHex(const QByteArray& pHexValue)
{
	return decode(QByteArray::fromHex(pHexValue));
}


QSharedPointer<AuthenticatedAuxiliaryData> AuthenticatedAuxiliaryData::decode(const QByteArray& pBytes)
{
	auto auxDate = decodeObject<AuthenticatedAuxiliaryDataInternal>(pBytes);

	if (!auxDate)
	{
		return nullptr;
	}

	QByteArrayList oids;
	for (int i = 0; i < sk_AuxDataTemplate_num(auxDate.data()); i++)
	{
		AuxDataTemplate* auxDataTemplate = sk_AuxDataTemplate_value(auxDate.data(), i);
		const auto oid = Asn1ObjectUtil::convertTo(auxDataTemplate->mAuxId);
		if (oids.contains(oid))
		{
			qCCritical(card) << "More than one AuxDataTemplate with OID" << oid;
			return nullptr;
		}
		oids += oid;
	}

	oids.removeOne(toByteArray(KnownOIDs::AuxilaryData::ID_COMMUNITY_ID));
	oids.removeOne(toByteArray(KnownOIDs::AuxilaryData::ID_DATE_OF_BIRTH));
	oids.removeOne(toByteArray(KnownOIDs::AuxilaryData::ID_DATE_OF_EXPIRY));
	if (!oids.isEmpty())
	{
		qCCritical(card) << "Unknown AuxDataTemplate with OID" << oids.first();
		return nullptr;
	}

	return QSharedPointer<AuthenticatedAuxiliaryData>(new AuthenticatedAuxiliaryData(auxDate));
}


QByteArray AuthenticatedAuxiliaryData::encode() const
{
	Q_ASSERT(mData);
	return encodeObject(mData.data());
}


bool AuthenticatedAuxiliaryData::hasValidityDate() const
{
	return getAuxDataTemplateFor(KnownOIDs::AuxilaryData::ID_DATE_OF_EXPIRY) != nullptr;
}


QDate AuthenticatedAuxiliaryData::getValidityDate() const
{
	if (auto auxData = getAuxDataTemplateFor(KnownOIDs::AuxilaryData::ID_DATE_OF_EXPIRY))
	{
		QByteArray extBytes = Asn1TypeUtil::encode(auxData->mExtInfo);

		auto validityDate = decodeObject<ValidityDate>(extBytes);
		if (validityDate != nullptr)
		{
			auto dateString = QString::fromLatin1(reinterpret_cast<char*>(validityDate->data), validityDate->length);
			return QDate::fromString(dateString, QStringLiteral("yyyyMMdd"));
		}
	}
	return QDate();
}


bool AuthenticatedAuxiliaryData::hasAgeVerificationDate() const
{
	return getAuxDataTemplateFor(KnownOIDs::AuxilaryData::ID_DATE_OF_BIRTH) != nullptr;
}


QDate AuthenticatedAuxiliaryData::getAgeVerificationDate() const
{
	if (auto auxData = getAuxDataTemplateFor(KnownOIDs::AuxilaryData::ID_DATE_OF_BIRTH))
	{
		QByteArray extBytes = Asn1TypeUtil::encode(auxData->mExtInfo);

		auto ageVerificationDate = decodeObject<AgeVerificationDate>(extBytes);
		if (ageVerificationDate != nullptr)
		{
			auto dateString = QString::fromLatin1(reinterpret_cast<char*>(ageVerificationDate->data), ageVerificationDate->length);
			return QDate::fromString(dateString, QStringLiteral("yyyyMMdd"));
		}
	}
	return QDate();
}


QString AuthenticatedAuxiliaryData::getRequiredAge(const QDate& pEffectiveDate) const
{
	const QDate ageVerificationDate = getAgeVerificationDate();
	if (!pEffectiveDate.isValid() || !ageVerificationDate.isValid())
	{
		return QString();
	}

	int age = pEffectiveDate.year() - ageVerificationDate.year();

	if (pEffectiveDate.month() <= ageVerificationDate.month() && (pEffectiveDate.month() != ageVerificationDate.month() || pEffectiveDate.day() < ageVerificationDate.day()))
	{
		--age;
	}

	return QString::number(age);
}


QString AuthenticatedAuxiliaryData::getRequiredAge() const
{
	return getRequiredAge(QDateTime::currentDateTimeUtc().date());
}


bool AuthenticatedAuxiliaryData::hasCommunityID() const
{
	return getAuxDataTemplateFor(KnownOIDs::AuxilaryData::ID_COMMUNITY_ID) != nullptr;
}


QByteArray AuthenticatedAuxiliaryData::getCommunityID() const
{
	if (auto auxData = getAuxDataTemplateFor(KnownOIDs::AuxilaryData::ID_COMMUNITY_ID))
	{
		QByteArray extBytes = Asn1TypeUtil::encode(auxData->mExtInfo);
		auto communityId = decodeObject<CommunityID>(extBytes);
		if (communityId != nullptr)
		{
			return QByteArray(reinterpret_cast<char*>(communityId->data), communityId->length).toHex();
		}
	}
	return QByteArray();
}


AuxDataTemplate* AuthenticatedAuxiliaryData::getAuxDataTemplateFor(KnownOIDs::AuxilaryData pData) const
{
	const auto& oid = toByteArray(pData);

	for (int i = 0; i < sk_AuxDataTemplate_num(mData.data()); i++)
	{
		AuxDataTemplate* auxDataTemplate = sk_AuxDataTemplate_value(mData.data(), i);
		if (Asn1ObjectUtil::convertTo(auxDataTemplate->mAuxId) == oid)
		{
			return auxDataTemplate;
		}
	}
	return nullptr;
}
