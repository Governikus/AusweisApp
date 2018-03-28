/*!
 *
 * \brief An interface for RemoteHandleImpl, meant to omit the
 * dependency between card_base and remote_device.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"

#include <QEnableSharedFromThis>
#include <QObject>


namespace governikus
{

class DataChannel;
class RemoteMessage;

class RemoteDispatcher
	: public QObject
	, public QEnableSharedFromThis<RemoteDispatcher>
{
	Q_OBJECT

	protected:
		RemoteDispatcher();

	public:
		virtual ~RemoteDispatcher();

		virtual const QString& getId() const = 0;
		virtual const QString& getContextHandle() const = 0;
		virtual void close() = 0;
		Q_INVOKABLE virtual void send(const QSharedPointer<const RemoteMessage>& pMessage) = 0;

	Q_SIGNALS:
		void fireReceived(const QSharedPointer<const RemoteMessage>& pMessage, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher);
		void fireClosed(GlobalStatus::Code pCloseCode, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher);
};

} /* namespace governikus */
