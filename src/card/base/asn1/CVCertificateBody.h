/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ASN1TemplateUtil.h"
#include "Chat.h"
#include "EcdsaPublicKey.h"
#include "Oid.h"

#include <QByteArray>
#include <QCryptographicHash>
#include <QDate>
#include <QHash>
#include <QSharedPointer>

#include <openssl/ec.h>


namespace governikus
{

using CVCertificateBody = struct certificateprofilebody_st
{
	ASN1_OCTET_STRING* mCertificateProfileIdentifier;
	ASN1_STRING* mCertificationAuthorityReference;
	EcdsaPublicKey* mPublicKey;
	ASN1_STRING* mCertificateHolderReference;
	CHAT* mChat;
	ASN1_OCTET_STRING* mEffectiveDate;
	ASN1_OCTET_STRING* mExpirationDate;
	STACK_OF(ASN1_TYPE) * mExtensions;

	static QSharedPointer<certificateprofilebody_st> decode(const QByteArray& pBytes);
	QByteArray encode();

	[[nodiscard]] QByteArray getCertificateProfileIdentifier() const;
	[[nodiscard]] QByteArray getCertificationAuthorityReference() const;

	[[nodiscard]] const EcdsaPublicKey& getPublicKey() const;
	[[nodiscard]] QByteArray getCertificateHolderReference() const;

	[[nodiscard]] const CHAT& getCHAT() const;

	void setCertificateExpirationDate(QDate date);
	[[nodiscard]] QDate getCertificateExpirationDate() const;

	void setCertificateEffectiveDate(QDate date);
	[[nodiscard]] QDate getCertificateEffectiveDate() const;

	[[nodiscard]] QCryptographicHash::Algorithm getHashAlgorithm() const;
	[[nodiscard]] QByteArray getExtension(const Oid& pOid) const;
};

DECLARE_ASN1_FUNCTIONS(CVCertificateBody)
DECLARE_ASN1_OBJECT(CVCertificateBody)

} // namespace governikus
