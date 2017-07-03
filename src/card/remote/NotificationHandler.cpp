/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#include "NotificationHandler.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(card_remote)


using namespace governikus;


void NotificationHandler::onReceived(const QByteArray& pDataBlock)
{
	const QSharedPointer<const RemoteCardNotification> received = mParser.parse(pDataBlock);
	if (received.isNull())
	{
		qCWarning(card_remote) << "Invalid notification received:" << pDataBlock;
	}
	else
	{
		Q_EMIT fireReceived(received);
	}
}


NotificationHandler::NotificationHandler(const QSharedPointer<DataChannel>& pDataChannel)
	: mDataChannel(pDataChannel)
	, mParser()
{
	connect(mDataChannel.data(), &DataChannel::fireClosed, this, &NotificationHandler::fireClosed);
	connect(mDataChannel.data(), &DataChannel::fireReceived, this, &NotificationHandler::onReceived);
}


NotificationHandler::~NotificationHandler()
{
	disconnect(mDataChannel.data(), &DataChannel::fireClosed, this, &NotificationHandler::fireClosed);
	disconnect(mDataChannel.data(), &DataChannel::fireReceived, this, &NotificationHandler::onReceived);

	mDataChannel->close();
}


void NotificationHandler::send(const QSharedPointer<const RemoteCardNotification>& pNotification)
{
	mDataChannel->send(pNotification->toJson().toJson());
}
