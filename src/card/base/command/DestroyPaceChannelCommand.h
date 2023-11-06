/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Command to destroy a Pace channel.
 */

#pragma once

#include "BaseCardCommand.h"
#include "CardConnectionWorker.h"

class test_DestroyPaceChannelCommand;

namespace governikus
{

class DestroyPaceChannelCommand
	: public BaseCardCommand
{
	Q_OBJECT
	friend class ::test_DestroyPaceChannelCommand;

	protected:
		void internalExecute() override;
		~DestroyPaceChannelCommand() override = default;

	public:
		explicit DestroyPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);
};

} // namespace governikus
