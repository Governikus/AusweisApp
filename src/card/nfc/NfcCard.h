/*!
 * \brief Implementation of \ref Card for NFC.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"

#include <QNearFieldTarget>


namespace governikus
{
class NfcCard
	: public Card
{
	Q_OBJECT

	private:
		bool mConnected;
		bool mIsValid;
		QNearFieldTarget* mNearFieldTarget;

	private Q_SLOTS:
		void onError(QNearFieldTarget::Error pError, const QNearFieldTarget::RequestId& pId);

	public:
		NfcCard(QNearFieldTarget* pNearFieldTarget);
		virtual ~NfcCard() override;

		bool isValid() const;
		bool invalidateTarget(QNearFieldTarget* pNearFieldTarget);

		virtual CardReturnCode connect() override;
		virtual CardReturnCode disconnect() override;
		virtual bool isConnected() override;

		virtual CardReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes) override;
};

} // namespace governikus
