/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
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
	, mPaceOutput(pPacePasswordId)
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
		mPaceOutput.setReturnCode(CardReturnCode::CARD_NOT_FOUND);
		setReturnCode(CardReturnCode::CARD_NOT_FOUND);
		return;
	}

	if (mPacePasswordId == PacePasswordId::PACE_PUK
			&& (getCardConnectionWorker()->getReaderInfo().getRetryCounter() > 0
			|| getCardConnectionWorker()->getReaderInfo().isPinDeactivated()))
	{
		mPaceOutput.setReturnCode(CardReturnCode::PIN_NOT_BLOCKED);
		setReturnCode(mPaceOutput.getReturnCode());
		return;
	}

	mPaceOutput = getCardConnectionWorker()->establishPaceChannel(mPacePasswordId, mPacePassword, mEffectiveChat, mCertificateDescription);
	setReturnCode(mPaceOutput.getReturnCode());
}
