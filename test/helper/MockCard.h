/*!
 * \brief Card mock for tests
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"

#include <QByteArray>
#include <QPair>
#include <QVector>

namespace governikus
{


typedef QPair<CardReturnCode, QByteArray> TransmitConfig;


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
		virtual ~MockCard() override;

		CardReturnCode connect() override;
		CardReturnCode disconnect() override;

		bool isConnected() override
		{
			return mConnected;
		}


		CardReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes) override;
};


} /* namespace governikus */
