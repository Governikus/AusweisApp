/*!
 * \brief Command to destroy a Pace channel.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"

namespace governikus
{

class CardConnection;

class DestroyPaceChannelCommand
	: public BaseCardCommand
{
	Q_OBJECT

	protected:
		virtual void internalExecute() override;
		virtual ~DestroyPaceChannelCommand() override;

	public:
		DestroyPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);
};

} /* namespace governikus */
