/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
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

		[[nodiscard]] bool isValid() const;
		bool invalidateTarget(const QNearFieldTarget* pNearFieldTarget);

		CardReturnCode establishConnection() override;
		CardReturnCode releaseConnection() override;
		bool isConnected() const override;
		void setProgressMessage(const QString& pMessage, int pProgress = -1) override;
		void setErrorMessage(const QString& pMessage) override;

		ResponseApduResult transmit(const CommandApdu& pCmd) override;

	Q_SIGNALS:
		void fireTargetError(QNearFieldTarget::Error pError);
		void fireSetProgressMessage(const QString& pMessage);
};

} // namespace governikus
