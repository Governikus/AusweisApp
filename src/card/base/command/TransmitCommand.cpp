/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "TransmitCommand.h"

#include "CardConnection.h"
#include "CardReturnCode.h"
#include "GlobalStatus.h"
#include "InputAPDUInfo.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;


TransmitCommand::TransmitCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
		const QVector<InputAPDUInfo>& pInputApduInfos)
	: BaseCardCommand(pCardConnectionWorker)
	, mInputApduInfos(pInputApduInfos)
	, mReaderName(pCardConnectionWorker->getReaderInfo().getName())
	, mOutputApduAsHex()
{
}


TransmitCommand::~TransmitCommand()
{

}


void TransmitCommand::internalExecute()
{
	Q_ASSERT(!mInputApduInfos.isEmpty());
	Q_ASSERT(mOutputApduAsHex.isEmpty());

	for (const auto& inputApduInfo : mInputApduInfos)
	{
		ResponseApdu response;
		mReturnCode = mCardConnectionWorker->transmit(inputApduInfo.getInputApdu(), response);
		if (mReturnCode != CardReturnCode::OK)
		{
			qCWarning(card) << "Transmit unsuccessful. Return code:" << CardReturnCodeUtil::toGlobalStatus(mReturnCode);

			return;
		}

		mOutputApduAsHex += response.getBuffer().toHex();
		if (!inputApduInfo.getAcceptableStatusCodes().isEmpty())
		{
			bool isAcceptable = false;

			for (const QByteArray& acceptableStatusCodeAsHex : inputApduInfo.getAcceptableStatusCodes())
			{
				// according to TR-03112-6 chapter 3.2.5
				if (response.getReturnCodeAsHex() == acceptableStatusCodeAsHex)
				{
					isAcceptable = true;
					break;
				}
			}

			if (!isAcceptable)
			{
				qCWarning(card) << "Transmit unsuccessful. StatusCode does not start with acceptable status code" << inputApduInfo.getAcceptableStatusCodes();
				mReturnCode = CardReturnCode::UNEXPECTED_TRANSMIT_STATUS;
				return;
			}
		}
	}
	qCDebug(card) << "transmit end";
	mReturnCode = CardReturnCode::OK;
}
