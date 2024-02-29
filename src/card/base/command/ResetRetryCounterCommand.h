/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"

class test_ResetRetryCounterCommand;

namespace governikus
{

class ResetRetryCounterCommand
	: public BaseCardCommand
{
	Q_OBJECT
	friend class ::test_ResetRetryCounterCommand;

	protected:
		void internalExecute() override;
		~ResetRetryCounterCommand() override = default;

	public:
		explicit ResetRetryCounterCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

};

} // namespace governikus
