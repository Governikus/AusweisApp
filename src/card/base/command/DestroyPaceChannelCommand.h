/*!
 * DestroyPaceChannelCommand.h
 *
 * \brief Command to destroy a Pace channel.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
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
		virtual ~DestroyPaceChannelCommand();

	public:
		DestroyPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);
};

} /* namespace governikus */
