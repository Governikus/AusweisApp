/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Component advertising the smartphone as card reader (SaC)
 * functionality on the server side. According to the concept this
 * is done by sending the message REMOTE_READER_OFFER as a UDP broadcast.
 */

#pragma once

#include "DatagramHandler.h"
#include "messages/Discovery.h"

#include <QObject>
#include <QScopedPointer>


namespace governikus
{

class RemoteReaderAdvertiser
	: public QObject
{
	Q_OBJECT

	protected:
		RemoteReaderAdvertiser() = default;

	public:
		~RemoteReaderAdvertiser() override = default;

		virtual void setPairing(bool)
		{
		}


};

class RemoteReaderAdvertiserImpl
	: public RemoteReaderAdvertiser
{
	Q_OBJECT

	private:
		const QScopedPointer<DatagramHandler> mHandler;
		const int mTimerId;
		Discovery mDiscovery;

		void timerEvent(QTimerEvent* pEvent) override;
		void sendDiscovery();

	public:
		~RemoteReaderAdvertiserImpl() override;
		RemoteReaderAdvertiserImpl(const QString& pIfdName, const QString& pIfdId, quint16 pPort, bool pPairing = false, int pTimerInterval = 1000);

		void setPairing(bool pEnabled) override;
};


} // namespace governikus
