/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MockQHttpConnection.h"

using namespace governikus;

MockQHttpConnection::MockQHttpConnection()
	: QHttpConnection(nullptr)
{
}


MockQHttpConnection::~MockQHttpConnection()
{
}


void MockQHttpConnection::write(const QByteArray& data)
{
	mContent.append(data);
}
