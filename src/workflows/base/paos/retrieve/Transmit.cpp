/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "Transmit.h"

using namespace governikus;

Transmit::Transmit()
	: PaosMessage(PaosType::TRANSMIT)
	, mSlotHandle()
	, mInputApduInfos()
{
}


Transmit::~Transmit() = default;
