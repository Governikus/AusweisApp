/*!
 * \brief Implementation of \ref Card for NFC.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"

#include <QNearFieldTarget>
#include <QScopedPointer>


namespace governikus
{
class NfcCard
	: public Card
{
	Q_OBJECT

	private:
		bool mConnected;
		bool mIsValid;
		QScopedPointer<QNearFieldTarget> mNearFieldTarget;

	public:
		explicit NfcCard(QNearFieldTarget* pNearFieldTarget);

		bool isValid() const;
		bool invalidateTarget(QNearFieldTarget* pNearFieldTarget);

		CardReturnCode connect() override;
		CardReturnCode disconnect() override;
		bool isConnected() override;
		void setProgressMessage(const QString& pMessage, int pProgress = -1) override;

		ResponseApduResult transmit(const CommandApdu& pCmd) override;

	Q_SIGNALS:
		void fireTargetError(QNearFieldTarget::Error pError);
		void fireSetProgressMessage(const QString& pMessage);
};

} // namespace governikus
