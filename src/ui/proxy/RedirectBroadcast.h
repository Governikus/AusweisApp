/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "PortWrapper.h"

#include <QNetworkDatagram>
#include <QObject>


namespace governikus
{
class RedirectBroadcast
	: public QObject
{
	Q_OBJECT

	private:
		PortWrapper mPortWrapper;

		void redirect(const QNetworkDatagram& pDatagram);

	public:
		explicit RedirectBroadcast(const QNetworkDatagram& pDatagram, quint16 pLocalPort);
		~RedirectBroadcast() override;
};

} // namespace governikus
