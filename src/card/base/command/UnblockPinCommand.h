/*!
 * UnblockPinCommand.h
 *
 * \brief Command to unblock the PIN of a card.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
		virtual ~UnblockPinCommand();

	public:
		UnblockPinCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker, const QString& pPuk);

};

} /* namespace governikus */
