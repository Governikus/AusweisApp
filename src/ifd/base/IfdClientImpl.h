/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdClient.h"
#include "IfdConnector.h"
#include "IfdList.h"

#include <QPointer>

namespace governikus
{


class IfdClientImpl
	: public IfdClient
{
	Q_OBJECT

	private:
		QMap<QByteArray, int> mErrorCounter;
		QThread mIfdConnectorThread;
		QPointer<IfdConnector> mIfdConnector;
		QList<QSharedPointer<IfdListEntry>> mIfdConnectorPending;
		QByteArrayList mConnectedDeviceIds;

		void bootstrapConnectorThread();
		void shutdownConnectorThread();
		QSharedPointer<IfdListEntry> mapToAndTakeConnectorPending(const QByteArray& pIfdId);

	protected Q_SLOTS:
		void onDispatcherCreated(const QByteArray& pIfdId, const QSharedPointer<IfdDispatcherClient>& pDispatcher);
		void onDispatcherError(const QByteArray& pIfdId, IfdErrorCode pErrorCode);
		void onDispatcherDestroyed(GlobalStatus::Code pCloseCode, const QByteArray& pId);

	public:
		IfdClientImpl();
		~IfdClientImpl() override;

		Q_INVOKABLE void establishConnection(const QSharedPointer<IfdListEntry>& pEntry, const QByteArray& pPsk) override;

		QByteArrayList getConnectedDeviceIDs() const override;

};

} // namespace governikus
