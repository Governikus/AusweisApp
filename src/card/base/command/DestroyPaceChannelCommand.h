/*!
 * \brief Command to destroy a Pace channel.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"

class test_DestroyPaceChannelCommand;

namespace governikus
{

class DestroyPaceChannelCommand
	: public BaseCardCommand
{
	Q_OBJECT

	private:
		friend class ::test_DestroyPaceChannelCommand;

	protected:
		virtual void internalExecute() override;
		virtual ~DestroyPaceChannelCommand() override = default;

	public:
		explicit DestroyPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);
};

} // namespace governikus
