/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
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
		const QVector<InputAPDUInfo>& pInputApduInfos,
		const QString& pSlotHandle)
	: BaseCardCommand(pCardConnectionWorker)
	, mInputApduInfos(pInputApduInfos)
	, mSlotHandle(pSlotHandle)
	, mOutputApduAsHex()
{
}


bool TransmitCommand::isAcceptable(const InputAPDUInfo& pInputApduInfo, const ResponseApdu& pResponse)
{
	if (pInputApduInfo.getAcceptableStatusCodes().isEmpty())
	{
		return true;
	}

	for (const QByteArray& acceptableStatusCodeAsHex : pInputApduInfo.getAcceptableStatusCodes())
	{
		// according to TR-03112-6 chapter 3.2.5
		if (pResponse.getReturnCodeAsHex().startsWith(acceptableStatusCodeAsHex))
		{
			return true;
		}
	}

	return false;
}


void TransmitCommand::internalExecute()
{
	Q_ASSERT(!mInputApduInfos.isEmpty());
	Q_ASSERT(mOutputApduAsHex.isEmpty());

	for (const auto& inputApduInfo : mInputApduInfos)
	{
		auto [returnCode, response] = mCardConnectionWorker->transmit(inputApduInfo.getInputApdu());
		mReturnCode = returnCode;
		if (mReturnCode != CardReturnCode::OK)
		{
			qCWarning(card) << "Transmit unsuccessful. Return code:" << CardReturnCodeUtil::toGlobalStatus(mReturnCode);

			return;
		}

		mOutputApduAsHex += response.getBuffer().toHex();
		if (isAcceptable(inputApduInfo, response))
		{
			continue;
		}

		qCWarning(card) << "Transmit unsuccessful. StatusCode does not start with acceptable status code" << inputApduInfo.getAcceptableStatusCodes();
		mReturnCode = CardReturnCode::UNEXPECTED_TRANSMIT_STATUS;
		return;
	}
	qCDebug(card) << "transmit end";
	mReturnCode = CardReturnCode::OK;
}
