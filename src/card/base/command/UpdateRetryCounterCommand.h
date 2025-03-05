/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
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
	friend class ::test_UpdateRetryCounterCommand;

	protected:
		void internalExecute() override;
		~UpdateRetryCounterCommand() override = default;

	public:
		explicit UpdateRetryCounterCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

};

} // namespace governikus
