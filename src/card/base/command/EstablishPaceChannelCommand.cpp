/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "EstablishPaceChannelCommand.h"


using namespace governikus;


EstablishPaceChannelCommand::EstablishPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
		PacePasswordId pPacePasswordId,
		const QByteArray& pPacePassword, const QByteArray& pEffectiveChat, const QByteArray& pCertificateDescription)
	: BaseCardCommand(pCardConnectionWorker)
	, mPacePasswordId(pPacePasswordId)
	, mPacePassword(pPacePassword)
	, mEffectiveChat(pEffectiveChat)
	, mCertificateDescription(pCertificateDescription)
	, mPaceOutput()
{
}


const EstablishPaceChannelOutput& EstablishPaceChannelCommand::getPaceOutput() const
{
	return mPaceOutput;
}


void EstablishPaceChannelCommand::internalExecute()
{
	if (!getCardConnectionWorker()->getReaderInfo().hasEid())
	{
		mPaceOutput.setPaceReturnCode(CardReturnCode::CARD_NOT_FOUND);
		setReturnCode(CardReturnCode::CARD_NOT_FOUND);
		return;
	}

	if (mPacePasswordId == PacePasswordId::PACE_PUK
			&& (getCardConnectionWorker()->getReaderInfo().getRetryCounter() > 0
			|| getCardConnectionWorker()->getReaderInfo().isPinDeactivated()))
	{
		mPaceOutput.setPaceReturnCode(CardReturnCode::PIN_NOT_BLOCKED);
		setReturnCode(mPaceOutput.getPaceReturnCode());
		return;
	}

	if (getCardConnectionWorker()->getReaderInfo().isSoftwareSmartEid())
	{
		Q_ASSERT(mPacePasswordId == PacePasswordId::PACE_PIN);
		mPaceOutput = getCardConnectionWorker()->prepareIdentification(mEffectiveChat);
	}
	else
	{
		mPaceOutput = getCardConnectionWorker()->establishPaceChannel(mPacePasswordId, mPacePassword, mEffectiveChat, mCertificateDescription);
	}
	setReturnCode(mPaceOutput.getPaceReturnCode());
}
