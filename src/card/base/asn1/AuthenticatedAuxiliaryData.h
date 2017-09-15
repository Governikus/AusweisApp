/*!
 * AuthenticatedAuxiliaryData.h
 *
 * \brief Implementation of AuthenticatedAuxiliaryData.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "ASN1TemplateUtil.h"

#include <openssl/asn1t.h>

#include <QDate>
#include <QSharedPointer>
#include <QString>


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


DECLARE_STACK_OF(AuxDataTemplate)


struct AuthenticatedAuxiliaryData
	: stack_st_AuxDataTemplate
{
	static QSharedPointer<AuthenticatedAuxiliaryData> fromHex(const QByteArray& pHexValue);
	static QSharedPointer<AuthenticatedAuxiliaryData> decode(const QByteArray& pBytes);
	QByteArray encode();

	bool hasValidityDate() const;
	QDate getValidityDate() const;

	bool hasAgeVerificationDate() const;
	QDate getAgeVerificationDate() const;
	QString getRequiredAge() const;

	bool hasCommunityID() const;
	QByteArray getCommunityID() const;

	private:
		AuxDataTemplate* getAuxDataTemplateFor(const QByteArray& pOid) const;

};


DECLARE_ASN1_FUNCTIONS(AuthenticatedAuxiliaryData)
DECLARE_ASN1_OBJECT(AuthenticatedAuxiliaryData)

} /* namespace governikus */
