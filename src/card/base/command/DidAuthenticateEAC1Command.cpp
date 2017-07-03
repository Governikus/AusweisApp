/*!
 * DidAuthenticateEAC1Command.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "BaseCardCommand.h"
#include "CardConnection.h"
#include "DidAuthenticateEAC1Command.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;


DidAuthenticateEAC1Command::DidAuthenticateEAC1Command(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
	: BaseCardCommand(pCardConnectionWorker)
	, mChallenge()
{
}


DidAuthenticateEAC1Command::~DidAuthenticateEAC1Command()
{

}


void DidAuthenticateEAC1Command::internalExecute()
{
	GetChallengeResponse response;
	mReturnCode = mCardConnectionWorker->transmit(GetChallengeBuilder().build(), response);
	if (mReturnCode != ReturnCode::OK || response.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "GetChallenge failed";
		mReturnCode = ReturnCode::GET_CHALLENGE_FAILED;
		return;
	}
	mChallenge = response.getChallenge();
}
