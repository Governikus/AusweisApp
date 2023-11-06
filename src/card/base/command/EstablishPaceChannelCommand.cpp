/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "EstablishPaceChannelCommand.h"

#include "apdu/CommandApdu.h"


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

	if (mPacePasswordId == PacePasswordId::PACE_PUK && getReturnCode() == CardReturnCode::OK)
	{
		const CommandApdu cmdApdu(Ins::RESET_RETRY_COUNTER, CommandApdu::UNBLOCK, CommandApdu::PIN);
		auto [returnCode, response] = getCardConnectionWorker()->transmit(cmdApdu);
		setReturnCode(returnCode);
		if (getReturnCode() == CardReturnCode::OK && response.getSW1() == SW1::ERROR_COMMAND_NOT_ALLOWED)
		{
			getCardConnectionWorker()->setPukInoperative();
			mPaceOutput.setPaceReturnCode(CardReturnCode::PUK_INOPERATIVE);
			setReturnCode(mPaceOutput.getPaceReturnCode());
		}
	}
}
