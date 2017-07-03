/*!
 * EstablishPaceChannelCommand.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "CardConnection.h"
#include "EstablishPaceChannelCommand.h"


using namespace governikus;


EstablishPaceChannelCommand::EstablishPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
		PACE_PIN_ID pPacePinId,
		const QString& pPacePin, const QByteArray& pEffectiveChat, const QByteArray& pCertificateDescription)
	: BaseCardCommand(pCardConnectionWorker)
	, mPacePinId(pPacePinId)
	, mPacePin(pPacePin)
	, mEffectiveChat(pEffectiveChat)
	, mCertificateDescription(pCertificateDescription)
{
	// This command only supports PIN and CAN.
	Q_ASSERT(mPacePinId == PACE_PIN_ID::PACE_PIN || mPacePinId == PACE_PIN_ID::PACE_CAN);
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
	mReturnCode = mCardConnectionWorker->establishPaceChannel(mPacePinId, mPacePin, mEffectiveChat, mCertificateDescription, mPaceOutput);
}
