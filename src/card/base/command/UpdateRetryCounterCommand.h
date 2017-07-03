/*!
 * UpdateRetryCounterCommand.h
 *
 * \brief Command to update the retry counter of a card.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
		virtual ~UpdateRetryCounterCommand();

	public:
		UpdateRetryCounterCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

};

} /* namespace governikus */
