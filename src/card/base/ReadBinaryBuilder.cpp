/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ReadBinaryBuilder.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card)

ReadBinaryBuilder::ReadBinaryBuilder(uint pOffset, int pLe)
	: CommandApduBuilder()
	, mOffset(pOffset)
	, mLe(pLe)
{
}


CommandApdu ReadBinaryBuilder::build()
{
	static const char INS = char(0xB0);
	return CommandApdu(CommandApdu::CLA, INS, static_cast<char>((mOffset & 0xff00) >> 8), static_cast<char>(mOffset & 0xff), QByteArray(), mLe);
}
