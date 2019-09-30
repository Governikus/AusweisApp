/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
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


CardReturnCode MockCard::transmit(const CommandApdu& pCmd, ResponseApdu& pRes)
{
	Q_UNUSED(pCmd)
	if (mCardConfig.mTransmits.isEmpty())
	{
		qFatal("No (more) response APDU configured, but a(nother) command transmitted");
	}
	QPair<CardReturnCode, QByteArray> config = mCardConfig.mTransmits.takeFirst();
	pRes.setBuffer(config.second);
	return config.first;
}


void MockCard::setConnected(bool pConnected)
{
	mConnected = pConnected;
}
