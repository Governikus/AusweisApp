/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "Msg.h"

using namespace governikus;

Msg::Msg(const MsgType& pType, const QByteArray& pData)
	: mType(pType)
	, mData(pData)
{
}


Msg::Msg()
	: Msg(MsgType::INTERNAL_ERROR, QByteArray())
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


#if !defined(QT_NO_DEBUG) && __has_include(<QTest>)
#include <QTest>
char* governikus::toString(const Msg& pMsg)
{
	return QTest::toString(QByteArray(pMsg));
}


#endif
