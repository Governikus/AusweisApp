/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "Apdu.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card)

Apdu::Apdu(const QByteArray& pBuffer)
	: mBuffer(pBuffer)
{
}


int Apdu::length() const
{
	return mBuffer.size();
}


bool Apdu::isEmpty() const
{
	return mBuffer.isEmpty();
}


const QByteArray& Apdu::getBuffer() const
{
	return mBuffer;
}
