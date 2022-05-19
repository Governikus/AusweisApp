/*!
 * \brief Implementation of \ref Card for iOS.
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"


namespace governikus
{
struct IosCardPointer;

class IosCard
	: public Card
{
	Q_OBJECT

	private:
		IosCardPointer* const mCard;
		bool mConnected;

	public:
		explicit IosCard(IosCardPointer* pTag);
		~IosCard() override;

		[[nodiscard]] bool isValid() const;
		void invalidateTarget();

		CardReturnCode connect() override;
		CardReturnCode disconnect() override;
		bool isConnected() override;
		void setProgressMessage(const QString& pMessage, int pProgress = -1) override;

		ResponseApduResult transmit(const CommandApdu& pCmd) override;
};

} // namespace governikus
