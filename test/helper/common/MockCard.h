/*!
 * \brief Card mock for tests
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"

#include <QByteArray>
#include <QPair>
#include <QVector>

namespace governikus
{

using TransmitConfig = QPair<CardReturnCode, QByteArray>;


class MockCardConfig
{
	public:
		QVector<TransmitConfig> mTransmits;
		CardReturnCode mConnect = CardReturnCode::OK;
		CardReturnCode mDisconnect = CardReturnCode::OK;

		MockCardConfig(const QVector<TransmitConfig>& pTransmits = QVector<TransmitConfig>())
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

		CardReturnCode connect() override;
		CardReturnCode disconnect() override;

		bool isConnected() override
		{
			return mConnected;
		}


		ResponseApduResult transmit(const CommandApdu& pCmd) override;

		void setConnected(bool pConnected);
};


} // namespace governikus
