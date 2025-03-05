/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QHostAddress>

class test_DatagramHandlerImpl;

namespace governikus
{

class DatagramHandler
	: public QObject
{
	Q_OBJECT

	public:
		explicit DatagramHandler(bool pEnableListening = true);
		~DatagramHandler() override = default;
		[[nodiscard]] virtual bool isBound() const = 0;
		virtual void send(const QByteArray& pData) = 0;

	Q_SIGNALS:
		void fireNewMessage(const QByteArray& pData, const QHostAddress& pAddress);
};


} // namespace governikus
