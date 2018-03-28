/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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
	if (mReturnCode != CardReturnCode::OK || response.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "GetChallenge failed";
		return;
	}
	mChallenge = response.getChallenge();
}
