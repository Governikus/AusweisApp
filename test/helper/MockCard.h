/*!
 * MockCard.h
 *
 * \brief Card mock for tests
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "Card.h"

#include <QByteArray>
#include <QPair>
#include <QVector>

namespace governikus
{


typedef QPair<ReturnCode, QByteArray> TransmitConfig;


class MockCardConfig
{
	public:
		QVector<TransmitConfig> mTransmits;
		ReturnCode mConnect = ReturnCode::OK;
		ReturnCode mDisconnect = ReturnCode::OK;

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
		virtual ~MockCard();

		ReturnCode connect() override;
		ReturnCode disconnect() override;

		bool isConnected() override
		{
			return mConnected;
		}


		ReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes) override;


		/*!
		 * Not supported, returns ReturnCode::COMMAND_FAILED
		 */
		ReturnCode establishPaceChannel(PACE_PIN_ID pPinId, const QByteArray& pChat, const QByteArray& pCertificateDescription, EstablishPACEChannelOutput& pChannelOutput, int pTimeoutSeconds) override
		{
			Q_UNUSED(pPinId);
			Q_UNUSED(pChat);
			Q_UNUSED(pCertificateDescription);
			Q_UNUSED(pChannelOutput);
			Q_UNUSED(pTimeoutSeconds);
			return ReturnCode::COMMAND_FAILED;
		}


		/*!
		 * Not supported, returns ReturnCode::COMMAND_FAILED
		 */
		ReturnCode setEidPin(unsigned int pTimeoutSeconds) override
		{
			Q_UNUSED(pTimeoutSeconds);
			return ReturnCode::COMMAND_FAILED;
		}


};


} /* namespace governikus */
