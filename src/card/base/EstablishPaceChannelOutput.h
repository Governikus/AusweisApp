/*!
 * \brief Data object for output of card command EstablishPaceChannel
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/CertificateDescription.h"
#include "asn1/SecurityInfos.h"
#include "CardReturnCode.h"
#include "pace/EstablishPaceChannelCode.h"
#include "SmartCardDefinitions.h"


#include <QByteArray>


namespace governikus
{

/*!
 * As defined in TR-03119:
 *
 * EstablishPaceChannelOutput ::= SEQUENCE {
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


class EstablishPaceChannelOutput
{
	private:
		CardReturnCode mPaceReturnCode;
		QByteArray mEfCardAccess;
		QByteArray mCarCurr;
		QByteArray mCarPrev;
		QByteArray mIdIcc;
		QByteArray mStatusMseSetAt;

	public:
		EstablishPaceChannelOutput(CardReturnCode pPaceReturnCode = CardReturnCode::UNKNOWN);

		/**
		 * Defined in pcsc10_v2.02.08_amd1.1
		 */
		void parse(const QByteArray& pControlOutput, PacePasswordId pPasswordId);

		/**
		 * Defined in TR-03119
		 */
		QByteArray toCcid() const;
		void parseFromCcid(const QByteArray& pOutput, PacePasswordId pPasswordId);

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

		static CardReturnCode parseReturnCode(quint32 pPaceReturnCode, PacePasswordId pPasswordId);
		static EstablishPaceChannelErrorCode generateReturnCode(CardReturnCode pReturnCode);
};


}  // namespace governikus
