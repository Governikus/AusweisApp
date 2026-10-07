/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

#include "Msg.h"

using namespace governikus;

Msg::Msg(const MsgType& pType, const QByteArray& pData)
	: mType(pType)
	, mData(pData)
{
}


Msg::Msg()
	: Msg(MsgType::VOID, QByteArray())
{
}


Msg::operator QByteArray() const
{
	return mData;
}


Msg::operator MsgType() const
{
	return mType;
}


Msg::operator bool() const
{
	return !mData.isEmpty();
}
