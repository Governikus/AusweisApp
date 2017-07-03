/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "Chat.h"
#include "EcdsaPublicKey.h"

#include <QCryptographicHash>
#include <QDate>
#include <QMap>
#include <QSharedPointer>
#include <QString>

#include <openssl/ec.h>


namespace governikus
{

typedef struct CERTIFICATEEXTENSION_st
{
	ASN1_OBJECT* mOid;
	ASN1_OCTET_STRING* mObject1;
	ASN1_OCTET_STRING* mObject2;
	ASN1_OCTET_STRING* mObject3;
	ASN1_OCTET_STRING* mObject4;
	ASN1_OCTET_STRING* mObject5;
	ASN1_OCTET_STRING* mObject6;
	ASN1_OCTET_STRING* mObject7;
	ASN1_OCTET_STRING* mObject8;
} CERTIFICATEEXTENSION;
DECLARE_ASN1_FUNCTIONS(CERTIFICATEEXTENSION)

typedef struct certificateprofilebody_st
{
	ASN1_OCTET_STRING* mCertificateProfileIdentifier;
	ASN1_STRING* mCertificationAuthorityReference;
	EcdsaPublicKey* mPublicKey;
	ASN1_STRING* mCertificateHolderReference;
	CHAT* mChat;
	ASN1_OCTET_STRING* mEffectiveDate;
	ASN1_OCTET_STRING* mExpirationDate;
	STACK_OF(CERTIFICATEEXTENSION) * mExtensions;

	static QSharedPointer<certificateprofilebody_st> fromHex(const QString& pHexValue);
	static QSharedPointer<certificateprofilebody_st> decode(const QByteArray& pBytes);
	QByteArray encode();

	QByteArray getCertificateProfileIdentifier() const;
	QByteArray getCertificationAuthorityReference() const;

	const EcdsaPublicKey& getPublicKey() const;
	QByteArray getCertificateHolderReference() const;

	const CHAT& getCHAT() const;

	void setCertificateExpirationDate(QDate date);
	QDate getCertificateExpirationDate() const;

	void setCertificateEffectiveDate(QDate date);
	QDate getCertificateEffectiveDate() const;

	QCryptographicHash::Algorithm getHashAlgorithm() const;
	QMap<QByteArray, QByteArray> getExtensions() const;

} CVCertificateBody;

DECLARE_ASN1_FUNCTIONS(CVCertificateBody)
DECLARE_ASN1_OBJECT(CVCertificateBody)

}
