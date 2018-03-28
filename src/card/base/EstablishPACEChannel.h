/*!
 * \brief Data object for output of card command EstablishPACEChannel
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/ASN1TemplateUtil.h"
#include "asn1/CertificateDescription.h"
#include "asn1/SecurityInfos.h"
#include "Apdu.h"
#include "CardReturnCode.h"
#include "pace/EstablishPACEChannelCode.h"
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
		PACE_PASSWORD_ID mPasswordId;
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
		void setPasswordId(PACE_PASSWORD_ID pPasswordId);
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
		CardReturnCode mPaceReturnCode;
		QByteArray mEfCardAccess;
		QByteArray mCarCurr;
		QByteArray mCarPrev;
		QByteArray mIdIcc;
		QByteArray mStatusMseSetAt;

	public:
		EstablishPACEChannelOutput();

		/**
		 * Defined in pcsc10_v2.02.08_amd1.1
		 */
		void parse(const QByteArray& pControlOutput, PACE_PASSWORD_ID pPasswordId);

		/**
		 * Defined in TR-03119
		 */
		QByteArray toCcid() const;
		void parseFromCcid(const QByteArray& pOutput, PACE_PASSWORD_ID pPasswordId);

		CardReturnCode getPaceReturnCode() const;
		void setPaceReturnCode(CardReturnCode);

		QByteArray getEfCardAccess() const;
		void setEfCardAccess(const QByteArray&);

		QByteArray getIDicc() const;
		void setIdIcc(const QByteArray&);

		QByteArray getCARcurr() const;
		void setCarCurr(const QByteArray&);

		QByteArray getCARprev() const;
		void setCarPrev(const QByteArray&);

		QByteArray getMseStatusSetAt() const;
		void setStatusMseSetAt(const QByteArray& pStatusMseSetAt);

		static CardReturnCode parseReturnCode(quint32 pPaceReturnCode, PACE_PASSWORD_ID pPasswordId);
		static EstablishPACEChannelErrorCode generateReturnCode(CardReturnCode pReturnCode);
};


}  // namespace governikus
