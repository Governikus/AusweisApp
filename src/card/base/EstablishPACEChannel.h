/*!
 * \brief Data object for output of card command EstablishPACEChannel
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "asn1/ASN1TemplateUtil.h"
#include "asn1/CertificateDescription.h"
#include "asn1/SecurityInfos.h"
#include "Apdu.h"
#include "ReturnCode.h"
#include "SmartCardDefinitions.h"

#include <QByteArray>


namespace governikus
{


/*!
 * As defined in TR-03119:
 *
 * EstablishPACEChannelInput ::= SEQUENCE {
 *      passwordID             [1] INTEGER
 *      transmittedPassword    [2] NumericString OPTIONAL
 *      cHAT                   [3] OCTET STRING OPTIONAL
 *      certificateDescription [4] CertificateDescription OPTIONAL
 *      hashOID                [5] OBJECT IDENTIFIER OPTIONAL
 * }
 * CertificateDescription is defined in [TR-03110].
 */
struct ESTABLISHPACECHANNELINPUT
{
	ASN1_INTEGER* mPasswordID;
	ASN1_STRING* mTransmittedPassword;
	ASN1_OCTET_STRING* mCHAT;
	CertificateDescription* mCertificateDescription;
	ASN1_OBJECT* mHashOID;
};
DECLARE_ASN1_OBJECT(ESTABLISHPACECHANNELINPUT)


class EstablishPACEChannelBuilder
{
	private:
		PACE_PIN_ID mPinId;
		QByteArray mChat;
		QByteArray mCertificateDescription;

	public:
		EstablishPACEChannelBuilder();

		/**
		 * Defined in pcsc10_v2.02.08_amd1.1
		 */
		QByteArray createCommandData();

		/**
		 * Defined in BSI-TR-03119_V1_pdf
		 */
		CommandApdu createCommandDataCcid();

		void setCertificateDescription(const QByteArray& pCertificateDescription);
		void setChat(const QByteArray& pChat);
		void setPinId(PACE_PIN_ID pPinId);
};


/*!
 * As defined in TR-03119:
 *
 * EstablishPACEChannelOutput ::= SEQUENCE {
 *      errorCode        [1] OCTET STRING (SIZE(4))
 *      statusMSESetAT   [2] OCTET STRING (SIZE(2))
 *      efCardAccess     [3] SecurityInfos
 *      idPICC           [4] OCTET STRING OPTIONAL
 *      curCAR           [5] OCTET STRING OPTIONAL
 *      prevCAR          [6] OCTET STRING OPTIONAL
 * }
 * SecurityInfos is defined in [TR-03110].
 */
struct ESTABLISHPACECHANNELOUTPUT
{
	ASN1_OCTET_STRING* mErrorCode;
	ASN1_OCTET_STRING* mStatusMSESetAt;
	securityinfos_st* mEfCardAccess;
	ASN1_OCTET_STRING* mIdPICC;
	ASN1_OCTET_STRING* mCurCAR;
	ASN1_OCTET_STRING* mPrevCAR;
};
DECLARE_ASN1_OBJECT(ESTABLISHPACECHANNELOUTPUT)


class EstablishPACEChannelOutput
{
	private:
		ReturnCode mPaceReturnCode;
		QByteArray mEfCardAccess;
		QByteArray mCarCurr;
		QByteArray mCarPrev;
		QByteArray mIdIcc;


		static uint parseUSHORT(const QByteArray& pData, int pOffset);

		static QByteArray reverse(const QByteArray& pArrayToReverse);

	public:
		EstablishPACEChannelOutput();

		/**
		 * Defined in pcsc10_v2.02.08_amd1.1
		 */
		void parse(const QByteArray& pControlOutput, PACE_PIN_ID pPinId);

		/**
		 * Defined in TR-03119
		 */
		void parseFromCcid(const QByteArray& pOutput, PACE_PIN_ID pPinId);

		ReturnCode getPaceReturnCode() const;
		void setPaceReturnCode(ReturnCode);

		QByteArray getEfCardAccess() const;
		void setEfCardAccess(const QByteArray&);

		QByteArray getIDicc() const;
		void setIdIcc(const QByteArray&);

		QByteArray getCARcurr() const;
		void setCarCurr(const QByteArray&);

		QByteArray getCARprev() const;
		void setCarPrev(const QByteArray&);

		static ReturnCode parseReturnCode(quint32 pPaceReturnCode, PACE_PIN_ID pPinId);
};


}  // namespace governikus
