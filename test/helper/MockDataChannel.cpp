/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#include "MockDataChannel.h"

using namespace governikus;


MockDataChannel::MockDataChannel()
{
}


MockDataChannel::~MockDataChannel()
{
}


void MockDataChannel::close()
{
	Q_EMIT fireClosed(GlobalStatus::Code::RemoteReader_CloseCode_NormalClose);
}


void MockDataChannel::closeAbnormal()
{
	Q_EMIT fireClosed(GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose);
}


void MockDataChannel::send(const QByteArray& pDataBlock)
{
	Q_EMIT fireSend(pDataBlock);
}


void MockDataChannel::onReceived(const QByteArray& pDataBlock)
{
	Q_EMIT fireReceived(pDataBlock);
}
