/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ResponseApdu.h"
#include "asn1/ASN1TemplateUtil.h"


namespace governikus
{

class GAResponseApdu
{
	private:
		ResponseApdu mResponseApdu;

	protected:
		[[nodiscard]] bool isValid() const;
		[[nodiscard]] QByteArray getResponseData() const;

	public:
		explicit GAResponseApdu(const ResponseApdu& pResponseApdu);
		[[nodiscard]] bool isEmpty() const;
		[[nodiscard]] SW1 getSW1() const;
		[[nodiscard]] int getRetryCounter() const;
};


/*!
 * According to TR-03110-3 the ASN.1 the response APDU has
 * one protocol specific data:
 *
 * EncryptedNonce ::= APPLICATION [0x00] IMPLICIT OCTET_STRING
 */
using GA_ENCRYPTEDNONCEDATA = struct ga_encryptednoncedata_st
{
	ASN1_OCTET_STRING* mEncryptedNonce;
};
DECLARE_ASN1_OBJECT(GA_ENCRYPTEDNONCEDATA)


class GAEncryptedNonceResponse
	: public GAResponseApdu
{
	private:
		void parseDynamicAuthenticationData();
		QByteArray mEncryptedNonce;

	public:
		explicit GAEncryptedNonceResponse(const ResponseApdu& pResponseApdu);
		[[nodiscard]] const QByteArray& getEncryptedNonce() const;
};


/*!
 * According to TR-03110-3 the ASN.1 the response APDU has
 * one protocol specific data:
 *
 * MappingData ::= APPLICATION [0x02] IMPLICIT OCTET_STRING
 */
using GA_MAPNONCEDATA = struct ga_mapnoncedata_st
{
	ASN1_OCTET_STRING* mMappingData;
};
DECLARE_ASN1_OBJECT(GA_MAPNONCEDATA)


class GAMapNonceResponse
	: public GAResponseApdu
{
	private:
		void parseDynamicAuthenticationData();
		QByteArray mMappingData;

	public:
		explicit GAMapNonceResponse(const ResponseApdu& pResponseApdu);
		[[nodiscard]] const QByteArray& getMappingData() const;
};


/*!
 * According to TR-03110-3 the ASN.1 the response APDU has
 * one protocol specific data:
 *
 * EphemeralPublicKey ::= APPLICATION [0x04] IMPLICIT OCTET_STRING
 */
using GA_PERFORMKEYAGREEMENTDATA = struct ga_performkeyagreementdata_st
{
	ASN1_OCTET_STRING* mEphemeralPublicKey;
};
DECLARE_ASN1_OBJECT(GA_PERFORMKEYAGREEMENTDATA)


class GAPerformKeyAgreementResponse
	: public GAResponseApdu
{
	private:
		void parseDynamicAuthenticationData();
		QByteArray mEphemeralPublicKey;

	public:
		explicit GAPerformKeyAgreementResponse(const ResponseApdu& pResponseApdu);
		[[nodiscard]] const QByteArray& getEphemeralPublicKey() const;
};


/*!
 * According to TR-03110-3 the ASN.1 the response APDU has
 * three protocol specific data:
 *
 * AuthenticationToken ::= APPLICATION [0x00] IMPLICIT OCTET_STRING
 * CarCurr             ::= APPLICATION [0x00] IMPLICIT OCTET_STRING OPTIONAL
 * CarPrev             ::= APPLICATION [0x00] IMPLICIT OCTET_STRING OPTIONAL
 */
using GA_MUTUALAUTHENTICATIONDATA = struct ga_mutualauthenticationdata_st
{
	ASN1_OCTET_STRING* mAuthenticationToken;
	ASN1_OCTET_STRING* mCarCurr;
	ASN1_OCTET_STRING* mCarPrev;
};
DECLARE_ASN1_OBJECT(GA_MUTUALAUTHENTICATIONDATA)


class GAMutualAuthenticationResponse
	: public GAResponseApdu
{
	private:
		void parseDynamicAuthenticationData();
		QByteArray mAuthenticationToken;
		QByteArray mCarCurr;
		QByteArray mCarPrev;

	public:
		explicit GAMutualAuthenticationResponse(const ResponseApdu& pResponseApdu);
		[[nodiscard]] const QByteArray& getAuthenticationToken() const;
		[[nodiscard]] const QByteArray& getCarCurr() const;
		[[nodiscard]] const QByteArray& getCarPrev() const;
};


/*!
 * According to TR-03110-3 the ASN.1 the response APDU has
 * three protocol specific data:
 *
 * Nonce               ::= APPLICATION [0x01] IMPLICIT OCTET_STRING
 * AuthenticationToken ::= APPLICATION [0x02] IMPLICIT OCTET_STRING
 */
using GA_CHIPAUTHENTICATIONDATA = struct ga_chipauthenticationdata_st
{
	ASN1_OCTET_STRING* mNonce;
	ASN1_OCTET_STRING* mAuthenticationToken;
};
DECLARE_ASN1_OBJECT(GA_CHIPAUTHENTICATIONDATA)


class GAChipAuthenticationResponse
	: public GAResponseApdu
{
	private:
		void parseDynamicAuthenticationData();
		QByteArray mNonce;
		QByteArray mAuthenticationToken;

	public:
		explicit GAChipAuthenticationResponse(const ResponseApdu& pResponseApdu);
		[[nodiscard]] const QByteArray& getNonce() const;
		[[nodiscard]] const QByteArray& getAuthenticationToken() const;
};


}  // namespace governikus
