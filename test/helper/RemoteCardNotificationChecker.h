/*!
 * \brief Helper class to verify the content of remote card notification.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include "RemoteCardNotifications.h"


namespace governikus
{

class RemoteCardNotificationChecker
	: public NotificationVisitor
{
	public:
		RemoteCardNotificationChecker() = default;

		virtual ~RemoteCardNotificationChecker() = default;

		virtual void process(const QSharedPointer<RemoteReaderDiscoverCmd>& pNotification) override;

		virtual void process(const QSharedPointer<RemoteReaderOfferMsg>& pNotification) override;

		virtual void process(const QSharedPointer<GetApiLevelCmd>& pNotification) override;

		virtual void process(const QSharedPointer<SetApiLevelCmd>& pNotification) override;

		virtual void process(const QSharedPointer<ApiLevelMsg>& pNotification) override;

		virtual void process(const QSharedPointer<GetReaderListCmd>& pNotification) override;

		virtual void process(const QSharedPointer<ReaderListMsg>& pNotification) override;

		virtual void process(const QSharedPointer<ReaderMsg>& pNotification) override;

		virtual void process(const QSharedPointer<ConnectCmd>& pNotification) override;

		virtual void process(const QSharedPointer<DisconnectCmd>& pNotification) override;

		virtual void process(const QSharedPointer<TransmitCmd>& pNotification) override;

		virtual void process(const QSharedPointer<TransmitResponseMsg>& pNotification) override;

		virtual void process(const QSharedPointer<UnsupportedMsg>& pNotification) override;

};

} /* namespace governikus */
