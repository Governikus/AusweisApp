/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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


ReturnCode MockCard::connect()
{
	mConnected = mCardConfig.mConnect == ReturnCode::OK;
	return mCardConfig.mConnect;
}


ReturnCode MockCard::disconnect()
{
	mConnected = mCardConfig.mDisconnect == ReturnCode::OK;
	return mCardConfig.mDisconnect;
}


ReturnCode MockCard::transmit(const CommandApdu& pCmd, ResponseApdu& pRes)
{
	Q_UNUSED(pCmd);
	if (mCardConfig.mTransmits.isEmpty())
	{
		qFatal("No (more) response APDU configured, but a(nother) command transmitted");
	}
	QPair<ReturnCode, QByteArray> config = mCardConfig.mTransmits.takeFirst();
	pRes.setBuffer(config.second);
	return config.first;
}
