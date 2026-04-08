/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"
#include "apdu/ResponseApdu.h"


class test_ResetRetryCounterCommand;


namespace governikus
{

class ResetRetryCounterCommand
	: public BaseCardCommand
{
	Q_OBJECT
	friend class ::test_ResetRetryCounterCommand;

	private:
		StatusCode mStatusCode;

	protected:
		void internalExecute() override;
		~ResetRetryCounterCommand() override = default;

	public:
		explicit ResetRetryCounterCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);
		[[nodiscard]] StatusCode getStatusCode() const;

};

} // namespace governikus
