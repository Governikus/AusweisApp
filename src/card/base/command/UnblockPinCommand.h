/*!
 * \brief Command to unblock the PIN of a card.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"

namespace governikus
{

class CardConnection;

class UnblockPinCommand
	: public BaseCardCommand
{
	Q_OBJECT

	private:
		QString mPuk;

	protected:
		virtual void internalExecute() override;
		virtual ~UnblockPinCommand() override;

	public:
		UnblockPinCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker, const QString& pPuk);

};

} /* namespace governikus */
