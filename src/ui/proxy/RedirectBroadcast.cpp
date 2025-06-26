/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

#include "RedirectBroadcast.h"

#include <QHostAddress>
#include <QLoggingCategory>
#include <QNetworkProxy>
#include <QUdpSocket>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(rproxy)


RedirectBroadcast::RedirectBroadcast(const QNetworkDatagram& pDatagram, quint16 pLocalPort)
	: QObject()
	, mPortWrapper(pLocalPort)
{
	if (mPortWrapper.isEmpty())
	{
		deleteLater();
	}
	else
	{
		redirect(pDatagram);
	}
}


RedirectBroadcast::~RedirectBroadcast() = default;


void RedirectBroadcast::redirect(const QNetworkDatagram& pDatagram)
{
	QUdpSocket socket;
	socket.setProxy(QNetworkProxy::NoProxy);

	quint16 port = 0;
	while ((port = mPortWrapper.fetchPort()) > 0)
	{
		if (socket.writeDatagram(pDatagram.data(), QHostAddress::LocalHost, port))
		{
			socket.flush();
		}
		else
		{
			qCDebug(rproxy) << "Cannot redirect to port:" << port;
		}

		mPortWrapper.invalidate();
	}

	deleteLater();
}
