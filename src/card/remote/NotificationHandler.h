/*!
 * NotificationHandler.h
 *
 * \brief Class that dispatches incoming and outgoing remote card reader notifications.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include "DataChannel.h"
#include "NotificationParser.h"
#include "RemoteCardNotifications.h"

namespace governikus
{
class NotificationHandler
	: public QObject
{
	Q_OBJECT

	private:
		const QSharedPointer<DataChannel> mDataChannel;
		const NotificationParser mParser;

	private Q_SLOTS:
		void onReceived(const QByteArray& pDataBlock);

	public:
		NotificationHandler(const QSharedPointer<DataChannel>& pDataChannel);
		virtual ~NotificationHandler();

		Q_INVOKABLE void send(const QSharedPointer<const RemoteCardNotification>& pNotification);

	Q_SIGNALS:
		void fireReceived(const QSharedPointer<const RemoteCardNotification>& pNotification);
		void fireClosed(GlobalStatus::Code pCloseCode);
};

} /* namespace governikus */
