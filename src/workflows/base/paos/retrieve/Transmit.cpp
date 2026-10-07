/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
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


const QString& Transmit::getSlotHandle() const
{
	return mSlotHandle;
}


void Transmit::setSlotHandle(const QString& pSlotHandle)
{
	mSlotHandle = pSlotHandle;
}


const QList<InputAPDUInfo>& Transmit::getInputApduInfos() const
{
	return mInputApduInfos;
}


void Transmit::appendInputApduInfo(const InputAPDUInfo& pInfo)
{
	mInputApduInfos += pInfo;
}
