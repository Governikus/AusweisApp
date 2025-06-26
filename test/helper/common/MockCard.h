/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"

#include <QByteArray>
#include <QList>
#include <QPair>

namespace governikus
{

using TransmitConfig = QPair<CardReturnCode, QByteArray>;


class MockCardConfig
{
	public:
		QList<TransmitConfig> mTransmits;
		CardReturnCode mConnect = CardReturnCode::OK;
		CardReturnCode mDisconnect = CardReturnCode::OK;

		MockCardConfig(const QList<TransmitConfig>& pTransmits = QList<TransmitConfig>())
			: mTransmits(pTransmits)
		{
		}


};


class MockCard
	: public Card
{
	Q_OBJECT

	bool mConnected;
	MockCardConfig mCardConfig;

	public:
		MockCard(const MockCardConfig& pCardConfig);
		~MockCard() override;

		CardReturnCode establishConnection() override;
		CardReturnCode releaseConnection() override;

		bool isConnected() const override
		{
			return mConnected;
		}


		ResponseApduResult transmit(const CommandApdu& pCmd) override;

		EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId, int pPreferredPinLength, const QByteArray& pChat, const QByteArray& pCertificateDescription) override;

		void setConnected(bool pConnected);
};


} // namespace governikus
