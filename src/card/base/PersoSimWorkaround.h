/*!
 * \brief This class is only to mark it as a workaround for working with the PersoSim.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardConnectionWorker.h"
#include "Commands.h"

#include <QDataStream>

namespace governikus
{

class PersoSimWorkaround
{
	private:
		PersoSimWorkaround();
		~PersoSimWorkaround();
		Q_DISABLE_COPY(PersoSimWorkaround)

	public:
		/*
		 * The PersoSim engine needs a select on EF.CardAccess to properly process MSE:Set AT.
		 *
		 * This is now needed, because we read EF.CardAccess just once on card recognition and store it.
		 * Therefore sending an MSE:Set AT is now no longer preceded by selecting and reading EF.CardAccess.
		 * This change caused the error with PersoSim.
		 *
		 * As soon as PersoSim is fixed in that point, we will remove the workaround.
		 */
		static CardReturnCode sendingMseSetAt(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
		{
			ResponseApdu response;
			const CardReturnCode returnCode = pCardConnectionWorker->transmit(SelectBuilder(FileRef::efCardAccess()).build(), response);
			return (returnCode == CardReturnCode::COMMAND_FAILED && response.getReturnCode() != StatusCode::EMPTY) ? CardReturnCode::OK : returnCode;
		}


		/*
		 * The PersoSim engine sends the result bytes in the control output of PACE in little endian.
		 * So we try to parse it that way, if we get unknown return codes.
		 *
		 * As soon as PersoSim is fixed in that point, we will remove the workaround.
		 */
		static CardReturnCode parsingEstablishPACEChannelOutput(const QByteArray& pControlOutput, PACE_PASSWORD_ID pPasswordId)
		{
			quint32 paceReturnCode;
			QDataStream(pControlOutput.mid(0, 4)) >> paceReturnCode;
			return EstablishPACEChannelOutput::parseReturnCode(paceReturnCode, pPasswordId);
		}


		/*
		 * The PersoSim engine sends 6A80 on wrong CAN entry in PACE. This means "Invalid data" which is
		 * wrong. We interpret it as wrong CAN
		 *
		 * As soon as PersoSim is fixed in that point, we will remove the workaround.
		 */
		static bool isWrongCanEntry(QSharedPointer<ResponseApdu> pResponseApdu)
		{
			return pResponseApdu->getReturnCode() == StatusCode::INVALID_DATAFIELD;
		}


};

} /* namespace governikus */
