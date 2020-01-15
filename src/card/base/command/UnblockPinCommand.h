/*!
 * \brief Command to unblock the PIN of a card.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"

class test_UnblockPinCommand;

namespace governikus
{

class UnblockPinCommand
	: public BaseCardCommand
{
	Q_OBJECT

	private:
		friend class ::test_UnblockPinCommand;
		QString mPuk;

	protected:
		virtual void internalExecute() override;
		virtual ~UnblockPinCommand() override = default;

	public:
		explicit UnblockPinCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker, const QString& pPuk);

};

} // namespace governikus
