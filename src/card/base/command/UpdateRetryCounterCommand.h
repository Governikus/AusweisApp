/*!
 * \brief Command to update the retry counter of a card.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"

namespace governikus
{

class CardConnection;

class UpdateRetryCounterCommand
	: public BaseCardCommand
{
	Q_OBJECT

	protected:
		virtual void internalExecute() override;
		virtual ~UpdateRetryCounterCommand() override;

	public:
		UpdateRetryCounterCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

};

} /* namespace governikus */
