/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QEventLoop>
#include <QStringList>
#include <QWebSocket>
#include <functional>

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
		[[nodiscard]] bool isConnected() const;
		[[nodiscard]] QAbstractSocket::SocketState getState() const;
		bool waitForMessage(const std::function<bool(const QJsonObject&)>& pMessageMatcher);
		void sendMessage(const QString& pMessage);
};

} // namespace governikus
