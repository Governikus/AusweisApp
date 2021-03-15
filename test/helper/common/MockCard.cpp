/*!
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

#include "MockCard.h"

using namespace governikus;

MockCard::MockCard(const MockCardConfig& pCardConfig)
	: mConnected(false)
	, mCardConfig(pCardConfig)
{
}


MockCard::~MockCard()
{
}


CardReturnCode MockCard::connect()
{
	mConnected = mCardConfig.mConnect == CardReturnCode::OK;
	return mCardConfig.mConnect;
}


CardReturnCode MockCard::disconnect()
{
	mConnected = mCardConfig.mDisconnect == CardReturnCode::OK;
	return mCardConfig.mDisconnect;
}


ResponseApduResult MockCard::transmit(const CommandApdu& pCmd)
{
	Q_UNUSED(pCmd)
	if (mCardConfig.mTransmits.isEmpty())
	{
		qFatal("No (more) response APDU configured, but a(nother) command transmitted");
	}
	QPair<CardReturnCode, QByteArray> config = mCardConfig.mTransmits.takeFirst();
	return {config.first, ResponseApdu(config.second)};
}


void MockCard::setConnected(bool pConnected)
{
	mConnected = pConnected;
}
