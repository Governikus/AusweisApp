/*!
 * \brief Implementation of GeneralAuthenticate response APDUs.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/ASN1TemplateUtil.h"
#include "Apdu.h"


namespace governikus
{


class GAResponseApdu
	: public ResponseApdu
{
	private:
		virtual void parseDynamicAuthenticationData(const QByteArray& pDynamicAuthenticationData) = 0;

	public:
		GAResponseApdu();
		virtual ~GAResponseApdu() override;
		virtual void setBuffer(const QByteArray& pBuffer) override;

};


/*!
 * According to TR-03110-3 the ASN.1 the response APDU has
 * one protocol specific data:
 *
 * EncryptedNonce ::= APPLICATION [0x00] IMPLICIT OCTET_STRING
 */
typedef struct ga_encryptednoncedata_st
{
	ASN1_OCTET_STRING* mEncryptedNonce;
} GA_ENCRYPTEDNONCEDATA;
DECLARE_ASN1_OBJECT(GA_ENCRYPTEDNONCEDATA)


class GAEncryptedNonceResponse
	: public GAResponseApdu
{
	private:
		virtual void parseDynamicAuthenticationData(const QByteArray& pDynamicAuthenticationData) override;
		QByteArray mEncryptedNonce;

	public:
		GAEncryptedNonceResponse();
		virtual ~GAEncryptedNonceResponse() override;
		const QByteArray& getEncryptedNonce();

};


/*!
 * According to TR-03110-3 the ASN.1 the response APDU has
 * one protocol specific data:
 *
 * MappingData ::= APPLICATION [0x02] IMPLICIT OCTET_STRING
 */
typedef struct ga_mapnoncedata_st
{
	ASN1_OCTET_STRING* mMappingData;
} GA_MAPNONCEDATA;
DECLARE_ASN1_OBJECT(GA_MAPNONCEDATA)


class GAMapNonceResponse
	: public GAResponseApdu
{
	private:
		virtual void parseDynamicAuthenticationData(const QByteArray& pDynamicAuthenticationData) override;
		QByteArray mMappingData;

	public:
		GAMapNonceResponse();
		virtual ~GAMapNonceResponse() override;
		const QByteArray& getMappingData();

};


/*!
 * According to TR-03110-3 the ASN.1 the response APDU has
 * one protocol specific data:
 *
 * EphemeralPublicKey ::= APPLICATION [0x04] IMPLICIT OCTET_STRING
 */
typedef struct ga_performkeyagreementdata_st
{
	ASN1_OCTET_STRING* mEphemeralPublicKey;
} GA_PERFORMKEYAGREEMENTDATA;
DECLARE_ASN1_OBJECT(GA_PERFORMKEYAGREEMENTDATA)


class GAPerformKeyAgreementResponse
	: public GAResponseApdu
{
	private:
		virtual void parseDynamicAuthenticationData(const QByteArray& pDynamicAuthenticationData) override;
		QByteArray mEphemeralPublicKey;

	public:
		GAPerformKeyAgreementResponse();
		virtual ~GAPerformKeyAgreementResponse() override;
		const QByteArray& getEphemeralPublicKey();

};


/*!
 * According to TR-03110-3 the ASN.1 the response APDU has
 * three protocol specific data:
 *
 * AuthenticationToken ::= APPLICATION [0x00] IMPLICIT OCTET_STRING
 * CarCurr             ::= APPLICATION [0x00] IMPLICIT OCTET_STRING OPTIONAL
 * CarPrev             ::= APPLICATION [0x00] IMPLICIT OCTET_STRING OPTIONAL
 */
typedef struct ga_mutualauthenticationdata_st
{
	ASN1_OCTET_STRING* mAuthenticationToken;
	ASN1_OCTET_STRING* mCarCurr;
	ASN1_OCTET_STRING* mCarPrev;
} GA_MUTUALAUTHENTICATIONDATA;
DECLARE_ASN1_OBJECT(GA_MUTUALAUTHENTICATIONDATA)


class GAMutualAuthenticationResponse
	: public GAResponseApdu
{
	private:
		virtual void parseDynamicAuthenticationData(const QByteArray& pDynamicAuthenticationData) override;
		QByteArray mAuthenticationToken, mCarCurr, mCarPrev;

	public:
		GAMutualAuthenticationResponse();
		virtual ~GAMutualAuthenticationResponse() override;
		const QByteArray& getAuthenticationToken();
		const QByteArray& getCarCurr();
		const QByteArray& getCarPrev();

};


/*!
 * According to TR-03110-3 the ASN.1 the response APDU has
 * three protocol specific data:
 *
 * Nonce               ::= APPLICATION [0x01] IMPLICIT OCTET_STRING
 * AuthenticationToken ::= APPLICATION [0x02] IMPLICIT OCTET_STRING
 */
typedef struct ga_chipauthenticationdata_st
{
	ASN1_OCTET_STRING* mNonce;
	ASN1_OCTET_STRING* mAuthenticationToken;
} GA_CHIPAUTHENTICATIONDATA;
DECLARE_ASN1_OBJECT(GA_CHIPAUTHENTICATIONDATA)


class GAChipAuthenticationResponse
	: public GAResponseApdu
{
	private:
		virtual void parseDynamicAuthenticationData(const QByteArray& pDynamicAuthenticationData) override;
		QByteArray mNonce, mAuthenticationToken;

	public:
		GAChipAuthenticationResponse();
		virtual ~GAChipAuthenticationResponse() override;
		const QByteArray& getNonce();
		const QByteArray& getAuthenticationToken();

};


}  // namespace governikus
