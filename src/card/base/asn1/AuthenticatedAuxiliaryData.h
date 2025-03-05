/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Oid.h"

#include <openssl/asn1t.h>

#include <QDate>
#include <QSharedPointer>
#include <QString>


class test_AuthenticatedAuxiliaryData;


namespace governikus
{

/**
 * Defined in TR-03110-3, chapter D.2:
 *
 *
 * AuthenticatedAuxiliaryData ::=  [APPLICATION 0x07] IMPLICIT SET OF AuxDataTemplate
 *
 * AuxDataTemplate ::= [APPLICATION 0x13] IMPLICIT SEQUENCE {
 *         auxID            OBJECT IDENTIFIER,
 *         extInfo          ANY DEFINED BY auxID
 * }
 *
 * CommunityID ::= [APPLICATION 0x13] IMPLICIT OCTETSTRING
 *
 * ValidityDate ::= [APPLICATION 0x13] IMPLICIT Date
 *
 * AgeVerificationDate ::= [APPLICATION 0x13] IMPLICIT Date
 *
 * Date ::= NumericString (SIZE (8)) -- YYYYMMDD
 */


using AuxDataTemplate = struct auxdatatemplate_st
{
	ASN1_OBJECT* mAuxId;
	ASN1_TYPE* mExtInfo;
};


DEFINE_STACK_OF(AuxDataTemplate)
using AuthenticatedAuxiliaryDataInternal = STACK_OF(AuxDataTemplate);


class AuthenticatedAuxiliaryData
{
	friend class ::test_AuthenticatedAuxiliaryData;
	friend class QSharedPointer<AuthenticatedAuxiliaryData>;

	private:
		QSharedPointer<AuthenticatedAuxiliaryDataInternal> mData;

		explicit AuthenticatedAuxiliaryData(const QSharedPointer<AuthenticatedAuxiliaryDataInternal>& pData);
		[[nodiscard]] AuxDataTemplate* getAuxDataTemplateFor(const Oid& pOid) const;

		[[nodiscard]] QString getRequiredAge(const QDate& pEffectiveDate) const;

	public:
		static QSharedPointer<AuthenticatedAuxiliaryData> fromHex(const QByteArray& pHexValue);
		static QSharedPointer<AuthenticatedAuxiliaryData> decode(const QByteArray& pBytes);

		[[nodiscard]] bool hasValidityDate() const;
		[[nodiscard]] QDate getValidityDate() const;

		[[nodiscard]] bool hasAgeVerificationDate() const;
		[[nodiscard]] QDate getAgeVerificationDate() const;
		[[nodiscard]] QString getRequiredAge() const;

		[[nodiscard]] bool hasCommunityID() const;
		[[nodiscard]] QByteArray getCommunityID() const;
};

} // namespace governikus
