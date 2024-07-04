/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "TransmitCommand.h"

#include "CardReturnCode.h"
#include "GlobalStatus.h"
#include "InputAPDUInfo.h"

#include <QLoggingCategory>

#include <algorithm>


Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


TransmitCommand::TransmitCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
		const QList<InputAPDUInfo>& pInputApduInfos,
		const QString& pSlotHandle)
	: BaseCardCommand(pCardConnectionWorker)
	, mInputApduInfos(pInputApduInfos)
	, mSlotHandle(pSlotHandle)
	, mOutputApduAsHex()
	, mSecureMessagingStopped(false)
{
	connect(pCardConnectionWorker.data(), &CardConnectionWorker::fireSecureMessagingStopped, this, [this](){
			mSecureMessagingStopped = true;
		});
}


bool TransmitCommand::isAcceptable(const InputAPDUInfo& pInputApduInfo, const ResponseApdu& pResponse)
{
	const auto& acceptableStatusCodes = pInputApduInfo.getAcceptableStatusCodes();

	if (acceptableStatusCodes.isEmpty())
	{
		return true;
	}

	const auto& responseBytes = pResponse.getStatusBytes().toHex();
	return std::any_of(acceptableStatusCodes.constBegin(), acceptableStatusCodes.constEnd(),
			[&responseBytes](const auto& pCode)
			{
				return responseBytes.startsWith(pCode); // according to TR-03112-6 chapter 3.2.5
			});
}


void TransmitCommand::internalExecute()
{
	Q_ASSERT(!mInputApduInfos.isEmpty());
	Q_ASSERT(mOutputApduAsHex.isEmpty());

	for (const auto& inputApduInfo : mInputApduInfos)
	{
		auto [returnCode, response] = getCardConnectionWorker()->transmit(inputApduInfo.getInputApdu());
		setReturnCode(returnCode);
		if (getReturnCode() != CardReturnCode::OK)
		{
			qCWarning(card) << "Transmit unsuccessful. Return code:" << CardReturnCodeUtil::toGlobalStatus(getReturnCode());

			return;
		}

		if (response.isEmpty())
		{
			qCWarning(card) << "Transmit unsuccessful. Received empty response.";
			setReturnCode(CardReturnCode::COMMAND_FAILED);
			return;
		}

		mOutputApduAsHex += QByteArray(response).toHex();
		if (isAcceptable(inputApduInfo, response))
		{
			continue;
		}

		qCWarning(card) << "Transmit unsuccessful. StatusCode does not start with acceptable status code" << inputApduInfo.getAcceptableStatusCodes();
		setReturnCode(CardReturnCode::UNEXPECTED_TRANSMIT_STATUS);
		return;
	}
	qCDebug(card) << "transmit end";
	setReturnCode(CardReturnCode::OK);
}
