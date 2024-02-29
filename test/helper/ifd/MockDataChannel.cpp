/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "MockDataChannel.h"

#include <QUuid>


using namespace governikus;


MockDataChannel::MockDataChannel(bool pPairing)
	: mPairing(pPairing)
	, mId(QUuid::createUuid().toString())
	, mReceivedDataBlocks()
{
}


MockDataChannel::~MockDataChannel()
{
}


void MockDataChannel::close()
{
	Q_EMIT fireClosed(GlobalStatus::Code::No_Error);
}


bool MockDataChannel::isPairingConnection() const
{
	return mPairing;
}


const QString& MockDataChannel::getId() const
{
	return mId;
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


const QList<QByteArray>& MockDataChannel::getReceivedDataBlocks() const
{
	return mReceivedDataBlocks;
}
