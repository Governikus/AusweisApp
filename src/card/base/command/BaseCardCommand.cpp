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
	, mReturnCode(ReturnCode::UNKNOWN)
{
	Q_ASSERT(mCardConnectionWorker);
	qRegisterMetaType<QSharedPointer<BaseCardCommand> >("QSharedPointer<BaseCardCommand>");
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


ReturnCode BaseCardCommand::checkRetryCounterAndPrepareForPace(const QString& pCan)
{
	ReturnCode returnCode = mCardConnectionWorker->updateRetryCounter();
	if (returnCode != ReturnCode::OK)
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
			return ReturnCode::OK;

		default:
			qCWarning(card) << "Card blocked or deactivated.";
			return ReturnCode::PIN_BLOCKED;
	}
}
