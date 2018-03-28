/*!
 * \brief Class for parsing JSON data to remote card message objects.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QScopedPointer>
#include <QSharedPointer>


namespace governikus
{
class Discovery;
class RemoteMessage;

class RemoteMessageParser
{
	public:
		RemoteMessageParser();
		~RemoteMessageParser();

		QSharedPointer<const Discovery> parseDiscovery(const QJsonDocument& pJsonDocument) const;
		QSharedPointer<const RemoteMessage> parse(const QByteArray& pJsonData) const;
		QSharedPointer<const RemoteMessage> parse(const QJsonDocument& pJsonDocument) const;
};

} /* namespace governikus */
