/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "BaseCardCommand.h"
#include "CardConnection.h"
#include "asn1/SecurityInfos.h"

#include <QLoggingCategory>
#include <QSharedPointer>
#include <QThread>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;

BaseCardCommand::BaseCardCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
	: mCardConnectionWorker(pCardConnectionWorker)
	, mReturnCode(CardReturnCode::UNKNOWN)
{
	Q_ASSERT(mCardConnectionWorker);
}


BaseCardCommand::~BaseCardCommand()
{
}


void BaseCardCommand::execute()
{
	Q_ASSERT(thread() == QThread::currentThread());

	internalExecute();
	qDebug(card) << "ReturnCode of internal execute:" << mReturnCode;

	QSharedPointer<BaseCardCommand> command(this, &QObject::deleteLater);
	Q_EMIT commandDone(command);
}


CardReturnCode BaseCardCommand::checkRetryCounterAndPrepareForPace(const QString& pCan)
{
	CardReturnCode returnCode = mCardConnectionWorker->updateRetryCounter();
	if (returnCode != CardReturnCode::OK)
	{
		return returnCode;
	}

	switch (mCardConnectionWorker->getReaderInfo().getRetryCounter())
	{
		case 1: // CAN required
		{
			EstablishPACEChannelOutput output;
			return mCardConnectionWorker->establishPaceChannel(PACE_PIN_ID::PACE_CAN, pCan, output);
		}

		case 2:
		case 3: // only PIN required
			return CardReturnCode::OK;

		default:
			qCWarning(card) << "Card blocked or deactivated.";
			return CardReturnCode::PIN_BLOCKED;
	}
}


void BaseCardCommand::registerMetaTypes()
{
	static bool registered = false;
	if (!registered)
	{
		qRegisterMetaType<QSharedPointer<BaseCardCommand> >("QSharedPointer<BaseCardCommand>");
		registered = true;
	}
}
