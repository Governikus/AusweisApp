/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <functional>
#include <QEventLoop>
#include <QStringList>
#include <QWebSocket>

namespace governikus
{

class WebSocketHelper
	: public QObject
{
	Q_OBJECT

	private:
		const int mConnectionTiemout;
		QWebSocket mWebSocket;
		QStringList mInput;

		void connectWebsocket(int pPort);

	private Q_SLOTS:
		void onTextMessageReceived(const QString& pMessage);

	public:
		WebSocketHelper(int pPort, int pConnectionTimeout = 15000);

		QAbstractSocket::SocketState getState() const;
		bool waitForMessage(const std::function<bool(const QJsonObject&)>& pMessageMatcher);
		void sendMessage(const QString& pMessage);
};

} // namespace governikus
