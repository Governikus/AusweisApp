/*!
 * \brief Command to set the Eid PIN of a card.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
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
		QString mNewPin;
		quint8 mTimeoutSeconds;

	protected:
		virtual void internalExecute() override;
		virtual ~SetEidPinCommand() override;

	public:
		SetEidPinCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				const QString& pNewPin, quint8 pTimeoutSeconds);
};

} /* namespace governikus */
