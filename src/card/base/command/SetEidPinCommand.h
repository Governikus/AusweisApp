/*!
 * \brief Command to set the Eid PIN of a card.
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"

class test_SetEidPinCommand;
class MockSetEidPinCommand;

namespace governikus
{

class SetEidPinCommand
	: public BaseCardCommand
{
	Q_OBJECT
	friend class ::test_SetEidPinCommand;
	friend class ::MockSetEidPinCommand;

	private:
		QByteArray mNewPin;
		quint8 mTimeoutSeconds;
		ResponseApdu mResponseApdu;

	protected:
		void internalExecute() override;
		~SetEidPinCommand() override = default;

	public:
		explicit SetEidPinCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				const QByteArray& pNewPin, quint8 pTimeoutSeconds);

		[[nodiscard]] const ResponseApdu& getResponseApdu() const;
};

} // namespace governikus
