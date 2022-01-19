/*!
 * \brief Command to unblock the PIN of a card.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
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
		QByteArray mPuk;

	protected:
		void internalExecute() override;
		~UnblockPinCommand() override = default;

	public:
		explicit UnblockPinCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker, const QByteArray& pPuk);

};

} // namespace governikus
