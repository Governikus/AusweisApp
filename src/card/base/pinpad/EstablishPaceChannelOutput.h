/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "CardReturnCode.h"
#include "PaceResult.h"
#include "SmartCardDefinitions.h"
#include "apdu/ResponseApdu.h"
#include "asn1/SecurityInfos.h"
#include "pace/EstablishPaceChannelCode.h"


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
		PacePasswordId mPasswordId;
		CardReturnCode mReturnCode;
		quint32 mErrorCode;
		QByteArray mStatusMseSetAt;
		QByteArray mEfCardAccess;
		QByteArray mIdIcc;
		QByteArray mCarCurr;
		QByteArray mCarPrev;

		[[nodiscard]] static CardReturnCode toReturnCode(quint32 pErrorCode);

		void initMseStatusSetAt();
		void initEfCardAccess();

	public:
		explicit EstablishPaceChannelOutput(PacePasswordId pPasswordId = PacePasswordId::UNKNOWN, CardReturnCode pReturnCode = CardReturnCode::UNDEFINED);

		/**
		 * Defined in pcsc10_v2.02.08_amd1.1 section 2.5.12
		 */
		[[nodiscard]] bool parse(const QByteArray& pControlOutput);
		[[nodiscard]] bool parseResultCode(const QByteArray& pPaceOutput);

		/**
		 * Defined in pcsc10_v2.02.08_amd1.1 section 2.6.16
		 */
		[[nodiscard]] bool parseOutputData(const QByteArray& pOutput);

		/**
		 * Defined in BSI-TR-03119_V1_pdf
		 */
		[[nodiscard]] bool parseFromCcid(const QByteArray& pOutput);

		[[nodiscard]] PacePasswordId getPasswordId() const;

		[[nodiscard]] CardReturnCode getReturnCode() const;
		void setReturnCode(CardReturnCode pReturnCode);

		void setErrorCode(EstablishPaceChannelErrorCode pErrorCode);

		[[nodiscard]] PaceResult getPaceResult() const;
		[[nodiscard]] bool isUndefined() const;
		[[nodiscard]] bool isOk() const;
		[[nodiscard]] bool wrongPasswordUsed() const;

		[[nodiscard]] StatusCode getStatusCodeMseSetAt() const;
		void setStatusMseSetAt(const QByteArray& pStatusMseSetAt);

		[[nodiscard]] const QByteArray& getEfCardAccess() const;
		void setEfCardAccess(const QByteArray& pEfCardAccess);

		[[nodiscard]] const QByteArray& getIdIcc() const;
		void setIdIcc(const QByteArray& pIdIcc);

		[[nodiscard]] const QByteArray& getCarCurr() const;
		void setCarCurr(const QByteArray& pCarCurr);

		[[nodiscard]] const QByteArray& getCarPrev() const;
		void setCarPrev(const QByteArray& pCarPrev);

		[[nodiscard]] QByteArray toResultCode() const;
		[[nodiscard]] QByteArray toOutputData() const;
		[[nodiscard]] QByteArray toCcid() const;

#ifndef QT_NO_DEBUG
		bool operator==(const EstablishPaceChannelOutput& pOther) const
		{
			return !(mReturnCode != pOther.mReturnCode
				   || mEfCardAccess != pOther.mEfCardAccess
				   || mCarCurr != pOther.mCarCurr
				   || mCarPrev != pOther.mCarPrev
				   || mIdIcc != pOther.mIdIcc
				   || mStatusMseSetAt != pOther.mStatusMseSetAt);
		}


#endif


};


}  // namespace governikus
