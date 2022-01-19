/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "GetChallengeBuilder.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card)

/*
 * GetChallengeBuilder
 */

GetChallengeBuilder::GetChallengeBuilder()
{
}


CommandApdu GetChallengeBuilder::build()
{
	static const char INS = char(0x84);
	return CommandApdu(CommandApdu::CLA, INS, 0, 0, QByteArray(), 0x08);
}
