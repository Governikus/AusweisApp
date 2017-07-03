/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include <QEventLoop>
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
		QVector<QString> mInput;

		void connectWebsocket(int pPort);

	private Q_SLOTS:
		void onTextMessageReceived(const QString& pMessage);

	public:
		WebSocketHelper(int pPort, int pConnectionTimeout = 15000);

		QAbstractSocket::SocketState getState() const;
		bool waitForMessage(const std::function<bool(const QJsonObject&)>& pMessageMatcher);
		void sendMessage(const QString& pMessage);
};

} /* namespace governikus */
