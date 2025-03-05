/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"
#include "CardConnectionWorker.h"

#include <QString>


class test_DestroyPaceChannelCommand;


namespace governikus
{

class DestroyPaceChannelCommand
	: public BaseCardCommand
{
	Q_OBJECT
	friend class ::test_DestroyPaceChannelCommand;

	private:
		const QString mSlotHandle;
		bool mSecureMessagingStopped;

	protected:
		void internalExecute() override;
		~DestroyPaceChannelCommand() override = default;

	public:
		explicit DestroyPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				const QString& pSlotHandle);

		[[nodiscard]] const QString& getSlotHandle() const
		{
			return mSlotHandle;
		}


		[[nodiscard]] bool getSecureMessagingStopped() const
		{
			return mSecureMessagingStopped;
		}


};

} // namespace governikus
