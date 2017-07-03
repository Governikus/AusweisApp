/*!
 * SetEidPinCommand.h
 *
 * \brief Command to set the Eid PIN of a card.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "BaseCardCommand.h"

namespace governikus
{

class CardConnection;

class SetEidPinCommand
	: public BaseCardCommand
{
	Q_OBJECT

	private:
		/* QString mOldPin; */
		QString mNewPin;
		/* QString mCan; */
		unsigned int mTimeoutSeconds;

	protected:
		virtual void internalExecute() override;
		virtual ~SetEidPinCommand();

	public:
		SetEidPinCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				const QString& pNewPin, unsigned int pTimeoutSeconds = 90);
};

} /* namespace governikus */
