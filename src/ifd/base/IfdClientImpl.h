/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
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
		QMap<QString, int> mErrorCounter;
		QThread mIfdConnectorThread;
		QPointer<IfdConnector> mIfdConnector;
		QVector<QSharedPointer<IfdListEntry>> mIfdConnectorPending;
		QStringList mConnectedDeviceIds;

		void bootstrapConnectorThread();
		void shutdownConnectorThread();
		QSharedPointer<IfdListEntry> mapToAndTakeConnectorPending(const IfdDescriptor& pIfdDescriptor);

	protected Q_SLOTS:
		void onDispatcherCreated(const IfdDescriptor& pIfdDescriptor, const QSharedPointer<IfdDispatcherClient>& pDispatcher);
		void onDispatcherError(const IfdDescriptor& pIfdDescriptor, IfdErrorCode pErrorCode);
		void onDispatcherDestroyed(GlobalStatus::Code pCloseCode, const QString& pId);

	public:
		IfdClientImpl();
		~IfdClientImpl() override;

		Q_INVOKABLE void establishConnection(const QSharedPointer<IfdListEntry>& pEntry, const QString& pPsk) override;

		QStringList getConnectedDeviceIDs() const override;

};

} // namespace governikus
