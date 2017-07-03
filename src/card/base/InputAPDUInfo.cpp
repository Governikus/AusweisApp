/*!
 * InputAPDUInfo.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "InputAPDUInfo.h"

using namespace governikus;

InputAPDUInfo::InputAPDUInfo()
	: mInputApdu()
	, mAcceptableStatusCodes()
{
}


InputAPDUInfo::InputAPDUInfo(const QByteArray& pInputApdu, const QByteArrayList& pAcceptableStatusCodes)
	: mInputApdu(pInputApdu)
	, mAcceptableStatusCodes(pAcceptableStatusCodes)
{
}


bool InputAPDUInfo::isValid() const
{
	return !mInputApdu.isEmpty();
}
