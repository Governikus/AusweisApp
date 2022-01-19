/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ResetRetryCounterBuilder.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card)


ResetRetryCounterBuilder::ResetRetryCounterBuilder(const QByteArray& pPin)
	: CommandApduBuilder()
	, mPin(pPin)
{
}


CommandApdu ResetRetryCounterBuilder::build()
{
	static const char INS = 0x2c;
	// P1: 2 (change), 3 (unblock)
	const char p1 = mPin.isNull() ? char(3) : char(2);
	// P2: 3 (PIN) (2 (CAN) -- not used)
	// data: new PIN, when changing
	return CommandApdu(CommandApdu::CLA, INS, p1, 3, mPin);
}
