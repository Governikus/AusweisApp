/*!
 * \brief Implementation of \ref Card for iOS.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
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

		void waitForRequestCompleted(const bool& pCondition) const;

	public:
		explicit IosCard(IosCardPointer* pTag);
		virtual ~IosCard() override;

		bool isValid() const;
		void invalidateTarget();

		virtual CardReturnCode connect() override;
		virtual CardReturnCode disconnect() override;
		virtual bool isConnected() override;
		virtual void setProgressMessage(const QString& pMessage, int pProgress = -1) override;

		virtual ResponseApduResult transmit(const CommandApdu& pCmd) override;
};

} // namespace governikus
