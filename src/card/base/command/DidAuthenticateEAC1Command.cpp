/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "DidAuthenticateEAC1Command.h"

#include "BaseCardCommand.h"
#include "CardConnection.h"
#include "GetChallengeBuilder.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


DidAuthenticateEAC1Command::DidAuthenticateEAC1Command(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
	: BaseCardCommand(pCardConnectionWorker)
	, mChallenge()
{
}


void DidAuthenticateEAC1Command::internalExecute()
{
	ResponseApdu response;
	mReturnCode = mCardConnectionWorker->transmit(GetChallengeBuilder().build(), response);
	if (mReturnCode != CardReturnCode::OK || response.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "GetChallenge failed";
		return;
	}

	if (response.getDataLength() != 8)
	{
		qCCritical(card) << "Challenge has wrong size. Expect 8 bytes, got" << response.getDataLength();
	}

	mChallenge = response.getData();
}
