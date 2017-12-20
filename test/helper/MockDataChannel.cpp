/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "MockDataChannel.h"

using namespace governikus;


MockDataChannel::MockDataChannel()
	: mReceivedDataBlocks()
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
	mReceivedDataBlocks += pDataBlock;

	Q_EMIT fireReceived(pDataBlock);
}


const QVector<QByteArray>& MockDataChannel::getReceivedDataBlocks() const
{
	return mReceivedDataBlocks;
}
