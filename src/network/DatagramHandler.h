/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include <QHostAddress>
#include <QNetworkAddressEntry>


class test_DatagramHandlerImpl;


namespace governikus
{

class DatagramHandler
	: public QObject
{
	Q_OBJECT

	private:
		[[nodiscard]] static bool isValidBroadcastInterface(const QNetworkInterface& pInterface);
		[[nodiscard]] static bool isValidAddressEntry(const QNetworkAddressEntry& pEntry);

	protected:
		static constexpr QLatin1StringView ipv6MulticastAddress {"ff02::178"};

	public:
		[[nodiscard]] static QList<QNetworkAddressEntry> getAllBroadcastEntries();
		[[nodiscard]] static QHostAddress getBroadcastAddress(const QNetworkAddressEntry& pEntry);

		explicit DatagramHandler(bool pEnableListening = true);
		~DatagramHandler() override = default;

		[[nodiscard]] virtual bool isBound() const = 0;
		virtual void send(const QByteArray& pData, const QList<QNetworkAddressEntry>& pEntries) = 0;

	Q_SIGNALS:
		void fireNewMessage(const QByteArray& pData, const QHostAddress& pAddress);
};


} // namespace governikus
