/*!
 * \brief Implementation of \ref Card for iOS.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"

#include <QMutex>
#include <QWaitCondition>


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
		QWaitCondition mWaitCondition;
		QMutex mCallbackDone;

	public:
		explicit IosCard(IosCardPointer* pTag);
		virtual ~IosCard() override;

		bool isValid() const;
		bool invalidateTarget();

		virtual CardReturnCode connect() override;
		virtual CardReturnCode disconnect() override;
		virtual bool isConnected() override;
		virtual void setProgressMessage(const QString& pMessage) override;

		virtual CardReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes) override;

	Q_SIGNALS:
		void fireConnectFailed();
		void fireTransmitFailed();
};

} // namespace governikus
