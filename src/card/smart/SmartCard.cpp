/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "SmartCard.h"


using namespace governikus;


SmartCard::SmartCard()
	: Card()
	, mSmartManager()
{

}


CardReturnCode SmartCard::establishConnection()
{
	if (!isConnected())
	{
		Q_ASSERT(!mSmartManager);
		mSmartManager = SmartManager::get();
		return CardReturnCode::OK;
	}

	return CardReturnCode::COMMAND_FAILED;
}


CardReturnCode SmartCard::releaseConnection()
{
	if (isConnected())
	{
		mSmartManager.reset();
		return CardReturnCode::OK;
	}

	return CardReturnCode::COMMAND_FAILED;
}


bool SmartCard::isConnected() const
{
	return !mSmartManager.isNull();
}


ResponseApduResult SmartCard::transmit(const CommandApdu& pCmd)
{
	Q_ASSERT(mSmartManager);
	return mSmartManager->transmit(pCmd);
}


EstablishPaceChannelOutput SmartCard::prepareIdentification(const QByteArray& pChat)
{
	Q_ASSERT(mSmartManager);
	return mSmartManager->prepareIdentification(pChat);
}


ResponseApduResult SmartCard::getChallenge()
{
	Q_ASSERT(mSmartManager);
	return mSmartManager->challenge();
}


TerminalAndChipAuthenticationResult SmartCard::performTAandCA(
		const CVCertificateChain& pTerminalCvcChain,
		const QByteArray& pAuxiliaryData,
		const QByteArray& pSignature,
		const QByteArray& pPin,
		const QByteArray& pEphemeralPublicKey)
{
	Q_ASSERT(mSmartManager);
	return mSmartManager->performTAandCA(pTerminalCvcChain, pAuxiliaryData, pSignature, pPin, pEphemeralPublicKey);
}
