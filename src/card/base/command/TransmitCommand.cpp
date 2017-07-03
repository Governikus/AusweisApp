/*!
 * TransmitCommand.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "CardConnection.h"
#include "InputAPDUInfo.h"
#include "ReturnCode.h"
#include "ReturnCodeUtil.h"
#include "TransmitCommand.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;


TransmitCommand::TransmitCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
		const QVector<InputAPDUInfo>& pInputApduInfos)
	: BaseCardCommand(pCardConnectionWorker)
	, mInputApduInfos(pInputApduInfos)
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
		CommandApdu request(QByteArray::fromHex(inputApduInfo.getInputApdu()));

		mReturnCode = mCardConnectionWorker->transmit(request, response);
		if (mReturnCode != ReturnCode::OK)
		{
			qCWarning(card) << "Transmit unsuccessful. Return code " << ReturnCodeUtil::toString(mReturnCode);
			return;
		}

		mOutputApduAsHex.append(response.getBuffer().toHex());
		if (!inputApduInfo.getAcceptableStatusCodes().isEmpty())
		{
			bool isAcceptable = false;

			for (const QByteArray& acceptableStatusCodeAsHex : inputApduInfo.getAcceptableStatusCodes())
			{
				// according to TR-03112-6 chapter 3.2.5
				QByteArray acceptableStatusCode = QByteArray::fromHex(acceptableStatusCodeAsHex);
				QByteArray actualStatusCode(2, 0x00);
				actualStatusCode[0] = response.getSW1();
				actualStatusCode[1] = response.getSW2();

				if (actualStatusCode.startsWith(acceptableStatusCode))
				{
					isAcceptable = true;
					break;
				}
			}

			if (!isAcceptable)
			{
				qCWarning(card) << "Transmit unsuccessful. StatusCode does not start with acceptable status code" << inputApduInfo.getAcceptableStatusCodes();
				mReturnCode = ReturnCode::UNEXPECTED_TRANSMIT_STATUS;
				return;
			}
		}
	}
	qCDebug(card) << "transmit end";
	mReturnCode = ReturnCode::OK;
}
