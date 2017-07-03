/*!
 * AuthenticatedAuxiliaryData.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "AuthenticatedAuxiliaryData.h"
#include "KnownOIDs.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{

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
 * This defines the AuthenticatedAuxiliaryData object with the special tag 0x07.
 */
ASN1_ITEM_TEMPLATE(AuthenticatedAuxiliaryData) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_SET_OF | ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x07, AuthenticatedAuxiliaryData, AuxDataTemplate)
ASN1_ITEM_TEMPLATE_END(AuthenticatedAuxiliaryData)


IMPLEMENT_ASN1_FUNCTIONS(AuthenticatedAuxiliaryData)
IMPLEMENT_ASN1_OBJECT(AuthenticatedAuxiliaryData)


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


}  // namespace governikus


QSharedPointer<AuthenticatedAuxiliaryData> AuthenticatedAuxiliaryData::fromHex(const QByteArray& pHexValue)
{
	return decode(QByteArray::fromHex(pHexValue));
}


QSharedPointer<AuthenticatedAuxiliaryData> AuthenticatedAuxiliaryData::decode(const QByteArray& pBytes)
{
	auto auxDate = decodeObject<AuthenticatedAuxiliaryData>(pBytes);

	QByteArrayList oids;
	for (int i = 0; i < SKM_sk_num(AuxDataTemplate, auxDate.data()); i++)
	{
		AuxDataTemplate* auxDataTemplate = SKM_sk_value(AuxDataTemplate, auxDate.data(), i);
		const auto oid = Asn1ObjectUtil::convertTo(auxDataTemplate->mAuxId);
		if (oids.contains(oid))
		{
			qCritical(card) << "More than one AuxDataTemplate with OID" << oid;
			auxDate.clear();
			break;
		}
		oids.append(oid);
	}
	oids.removeOne(KnownOIDs::AuxilaryData::id_CommunityID);
	oids.removeOne(KnownOIDs::AuxilaryData::id_DateOfBirth);
	oids.removeOne(KnownOIDs::AuxilaryData::id_DateOfExpiry);
	if (!oids.isEmpty())
	{
		qCritical() << "Unknown AuxDataTemplate with oid" << oids.first();
		auxDate.clear();
	}

	return auxDate;
}


QByteArray AuthenticatedAuxiliaryData::encode()
{
	return encodeObject(this);
}


bool AuthenticatedAuxiliaryData::hasValidityDate() const
{
	return getAuxDataTemplateFor(KnownOIDs::AuxilaryData::id_DateOfExpiry) != nullptr;
}


QDate AuthenticatedAuxiliaryData::getValidityDate() const
{
	if (auto auxData = getAuxDataTemplateFor(KnownOIDs::AuxilaryData::id_DateOfExpiry))
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
	return getAuxDataTemplateFor(KnownOIDs::AuxilaryData::id_DateOfBirth) != nullptr;
}


QDate AuthenticatedAuxiliaryData::getAgeVerificationDate() const
{
	if (auto auxData = getAuxDataTemplateFor(KnownOIDs::AuxilaryData::id_DateOfBirth))
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


QString AuthenticatedAuxiliaryData::getRequiredAge() const
{
	QDateTime now = QDateTime::currentDateTime();
	QDate nowDateGMT = now.toUTC().date();
	QDate ageVerificationDate = getAgeVerificationDate();

	if (nowDateGMT.month() > ageVerificationDate.month() || (nowDateGMT.month() == ageVerificationDate.month() && nowDateGMT.day() >= ageVerificationDate.day()))
	{
		return QString::number(nowDateGMT.year() - ageVerificationDate.year());
	}
	else
	{
		return QString::number(nowDateGMT.year() - ageVerificationDate.year() - 1);
	}
}


bool AuthenticatedAuxiliaryData::hasCommunityID() const
{
	return getAuxDataTemplateFor(KnownOIDs::AuxilaryData::id_CommunityID) != nullptr;
}


QByteArray AuthenticatedAuxiliaryData::getCommunityID() const
{
	if (auto auxData = getAuxDataTemplateFor(KnownOIDs::AuxilaryData::id_CommunityID))
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


AuxDataTemplate* AuthenticatedAuxiliaryData::getAuxDataTemplateFor(const QByteArray& pOid) const
{
	// unfortunately OpenSSL cannot handle const pointers, so we cast it away
	AuthenticatedAuxiliaryData* notConstThis = const_cast<AuthenticatedAuxiliaryData*>(this);

	for (int i = 0; i < SKM_sk_num(AuxDataTemplate, notConstThis); i++)
	{
		AuxDataTemplate* auxDataTemplate = SKM_sk_value(AuxDataTemplate, notConstThis, i);
		if (Asn1ObjectUtil::convertTo(auxDataTemplate->mAuxId) == pOid)
		{
			return auxDataTemplate;
		}
	}
	return nullptr;
}
