/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "InputAPDUInfo.h"

using namespace governikus;

InputAPDUInfo::InputAPDUInfo(const QByteArray& pInputApdu)
	: mInputApdu(pInputApdu)
	, mAcceptableStatusCodes()
{
}
