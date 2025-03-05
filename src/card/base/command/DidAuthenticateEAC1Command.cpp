/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "DidAuthenticateEAC1Command.h"

#include "apdu/CommandApdu.h"

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
	auto [returnCode, response] = [this] {
				if (getCardConnectionWorker()->getReaderInfo().isSoftwareSmartEid())
				{
					return getCardConnectionWorker()->getChallenge();
				}
				else
				{
					CommandApdu cmdApdu(Ins::GET_CHALLENGE, CommandApdu::IMPLICIT, CommandApdu::IMPLICIT, QByteArray(), 8);
					return getCardConnectionWorker()->transmit(cmdApdu);
				}
			}
				();
	setReturnCode(returnCode);

	if (response.getStatusCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "GetChallenge failed";
		return;
	}

	if (response.getData().size() != 8)
	{
		qCCritical(card) << "Challenge has wrong size. Expect 8 bytes, got" << response.getData().size();
	}

	mChallenge = response.getData();
}
