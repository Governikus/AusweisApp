/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "InputAPDUInfo.h"

using namespace governikus;

InputAPDUInfo::InputAPDUInfo()
	: mInputApdu()
	, mAcceptableStatusCodes()
	, mUpdateRetryCounter(false)
{
}


InputAPDUInfo::InputAPDUInfo(const QByteArray& pInputApdu, bool pUpdateRetryCounter)
	: mInputApdu(pInputApdu)
	, mAcceptableStatusCodes()
	, mUpdateRetryCounter(pUpdateRetryCounter)
{
}
