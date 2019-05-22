/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "InputAPDUInfo.h"

using namespace governikus;

InputAPDUInfo::InputAPDUInfo()
	: mInputApdu()
	, mAcceptableStatusCodes()
{
}


InputAPDUInfo::InputAPDUInfo(const QByteArray& pInputApdu)
	: mInputApdu(pInputApdu)
	, mAcceptableStatusCodes()
{
}
