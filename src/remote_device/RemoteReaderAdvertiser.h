/*!
 * \brief Component advertising the remote reader functionality
 * on the server side. According to the concept this is done by
 * sending the message REMOTE_READER_OFFER as a UDP broadcast.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QJsonDocument>
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
		virtual ~RemoteReaderAdvertiser();
};


class DatagramHandler;


class RemoteReaderAdvertiserImpl
	: public RemoteReaderAdvertiser
{
	Q_OBJECT

	const QScopedPointer<DatagramHandler> mHandler;
	const int mTimerId;
	const QJsonDocument mDiscovery;

	void timerEvent(QTimerEvent* pEvent) override;

	public:
		virtual ~RemoteReaderAdvertiserImpl() override;
		RemoteReaderAdvertiserImpl(const QString& pIfdName, const QString& pIfdId, quint16 pPort, int pTimerInterval = 1000);
};


} /* namespace governikus */
