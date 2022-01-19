/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
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
	// This command only supports PIN and CAN.
	Q_ASSERT(mPacePasswordId == PacePasswordId::PACE_PIN || mPacePasswordId == PacePasswordId::PACE_CAN);
}


const EstablishPaceChannelOutput& EstablishPaceChannelCommand::getPaceOutput() const
{
	return mPaceOutput;
}


void EstablishPaceChannelCommand::internalExecute()
{
	mPaceOutput = mCardConnectionWorker->establishPaceChannel(mPacePasswordId, mPacePassword, mEffectiveChat, mCertificateDescription);
	mReturnCode = mPaceOutput.getPaceReturnCode();
}
