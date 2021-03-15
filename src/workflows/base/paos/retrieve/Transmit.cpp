/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "Transmit.h"

using namespace governikus;

Transmit::Transmit()
	: PaosMessage(PaosType::TRANSMIT)
	, mSlotHandle()
	, mInputApduInfos()
{
}


Transmit::~Transmit()
{
}
