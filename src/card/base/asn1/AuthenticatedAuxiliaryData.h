/*!
 * \brief Implementation of AuthenticatedAuxiliaryData.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ASN1TemplateUtil.h"

#include "KnownOIDs.h"

#include <openssl/asn1t.h>

#include <QDate>
#include <QSharedPointer>
#include <QString>

class test_AuxiliaryAuthenticatedData;

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


typedef struct auxdatatemplate_st
{
	ASN1_OBJECT* mAuxId;
	ASN1_TYPE* mExtInfo;

} AuxDataTemplate;

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
DECLARE_STACK_OF(AuxDataTemplate)
using AuthenticatedAuxiliaryDataInternal = stack_st_AuxDataTemplate;
#else
DEFINE_STACK_OF(AuxDataTemplate)
using AuthenticatedAuxiliaryDataInternal = STACK_OF(AuxDataTemplate);
#endif

class AuthenticatedAuxiliaryData
{
	friend class ::test_AuxiliaryAuthenticatedData;
	friend class QSharedPointer<AuthenticatedAuxiliaryData>;
	QSharedPointer<AuthenticatedAuxiliaryDataInternal> mData;

	AuthenticatedAuxiliaryData(const QSharedPointer<AuthenticatedAuxiliaryDataInternal>& pData);
	AuxDataTemplate* getAuxDataTemplateFor(KnownOIDs::AuxilaryData pData) const;

	QString getRequiredAge(const QDate& pEffectiveDate) const;

	public:
		static QSharedPointer<AuthenticatedAuxiliaryData> fromHex(const QByteArray& pHexValue);
		static QSharedPointer<AuthenticatedAuxiliaryData> decode(const QByteArray& pBytes);
		QByteArray encode() const;

		bool hasValidityDate() const;
		QDate getValidityDate() const;

		bool hasAgeVerificationDate() const;
		QDate getAgeVerificationDate() const;
		QString getRequiredAge() const;

		bool hasCommunityID() const;
		QByteArray getCommunityID() const;
};

} // namespace governikus
