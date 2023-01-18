/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "PinModifyOutput.h"

#include <QLoggingCategory>
#include <QtEndian>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


PinModifyOutput::PinModifyOutput()
	: mResponseApdu()
{
}


PinModifyOutput::PinModifyOutput(const ResponseApdu& pResponseApdu)
	: mResponseApdu(pResponseApdu)
{
}


QByteArray PinModifyOutput::toCcid() const
{
	return mResponseApdu;
}


const ResponseApdu& PinModifyOutput::getResponseApdu() const
{
	return mResponseApdu;
}
