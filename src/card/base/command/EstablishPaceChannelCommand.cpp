/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"
#include "EstablishPaceChannelCommand.h"


using namespace governikus;


EstablishPaceChannelCommand::EstablishPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
		PACE_PASSWORD_ID pPacePasswordId,
		const QString& pPacePassword, const QByteArray& pEffectiveChat, const QByteArray& pCertificateDescription)
	: BaseCardCommand(pCardConnectionWorker)
	, mPacePasswordId(pPacePasswordId)
	, mPacePassword(pPacePassword)
	, mEffectiveChat(pEffectiveChat)
	, mCertificateDescription(pCertificateDescription)
	, mPaceOutput()
{
	// This command only supports PIN and CAN.
	Q_ASSERT(mPacePasswordId == PACE_PASSWORD_ID::PACE_PIN || mPacePasswordId == PACE_PASSWORD_ID::PACE_CAN);
}


EstablishPaceChannelCommand::~EstablishPaceChannelCommand()
{
}


const EstablishPACEChannelOutput& EstablishPaceChannelCommand::getPaceOutput() const
{
	return mPaceOutput;
}


void EstablishPaceChannelCommand::internalExecute()
{
	mReturnCode = mCardConnectionWorker->establishPaceChannel(mPacePasswordId, mPacePassword, mEffectiveChat, mCertificateDescription, mPaceOutput);
}
