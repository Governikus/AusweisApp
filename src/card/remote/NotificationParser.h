/*!
 * NotificationParser.h
 *
 * \brief Class for parsing JSON data to remote card notification objects.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include "RemoteCardNotifications.h"

#include <QScopedPointer>

namespace governikus
{
class NotificationParserPrivate;

class NotificationParser
{
	Q_DECLARE_PRIVATE(NotificationParser)

	private:
		const QScopedPointer<NotificationParserPrivate> d_ptr;

	public:
		NotificationParser();

		~NotificationParser();

		QSharedPointer<const RemoteCardNotification> parse(const QByteArray& pJsonData) const;

		QSharedPointer<const RemoteCardNotification> parse(const QJsonDocument& pJsonDocument) const;
};

} /* namespace governikus */
