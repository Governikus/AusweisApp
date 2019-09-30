/*!
 * \brief This class is only to mark it as a workaround for working with the PersoSim.
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardConnectionWorker.h"
#include "SelectBuilder.h"

#include <QDataStream>

namespace governikus
{

class PersoSimWorkaround
{
	private:
		PersoSimWorkaround() = delete;
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

			if (response.getReturnCode() == StatusCode::EMPTY)
			{
				return CardReturnCode::RETRY_ALLOWED;
			}

			if (returnCode == CardReturnCode::COMMAND_FAILED)
			{
				return CardReturnCode::OK;
			}

			return returnCode;
		}


};

} // namespace governikus
