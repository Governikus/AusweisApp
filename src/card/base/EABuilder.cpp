/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "EABuilder.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card)

/*
 *  EABuilder
 */
EABuilder::EABuilder()
	: CommandApduBuilder()
	, mSignature()
{
}


void EABuilder::setSignature(const QByteArray& pData)
{
	mSignature = pData;
}


CommandApdu EABuilder::build()
{
	static const char INS = char(0x82);
	return CommandApdu(CommandApdu::CLA, INS, 0, 0, mSignature);
}
