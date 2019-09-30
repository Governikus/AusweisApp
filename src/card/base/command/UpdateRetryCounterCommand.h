/*!
 * \brief Command to update the retry counter of a card.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"

class test_UpdateRetryCounterCommand;

namespace governikus
{

class UpdateRetryCounterCommand
	: public BaseCardCommand
{
	Q_OBJECT

	private:
		friend class ::test_UpdateRetryCounterCommand;

	protected:
		virtual void internalExecute() override;
		virtual ~UpdateRetryCounterCommand() override = default;

	public:
		explicit UpdateRetryCounterCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

};

} // namespace governikus
