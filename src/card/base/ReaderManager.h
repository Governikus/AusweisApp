/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "command/CreateCardConnectionCommand.h"
#include "DeviceError.h"
#include "Reader.h"
#include "ReaderManagerWorker.h"
#include "RemoteClient.h"

#include <QPointer>
#include <QThread>

namespace governikus
{

class ReaderManager
	: public QObject
{
	Q_OBJECT

	private:
		QThread mThread;
		QPointer<ReaderManagerWorker> mWorker;
		QSharedPointer<RemoteClient> mRemoteClient;

	protected:
		ReaderManager();
		~ReaderManager();

	public:
		static ReaderManager& getInstance();

		/*!
		 * Initialize the reader manager service.
		 * The thread is started and the plug-ins are initialized, too.
		 */
		void init(const QSharedPointer<RemoteClient>& pRemoteClient = QSharedPointer<RemoteClient>());

		/*!
		 * Starts a scan for all device types.
		 */
		void startScanAll();

		/*!
		 * Starts a scan for devices if registered plugin don't scan anytime.
		 */
		void startScan(ReaderManagerPlugInType pType);

		/*!
		 * Stops scan for all device types.
		 */
		void stopScanAll();

		/*!
		 * Stops started scan for devices.
		 * Be aware that some plugins don't finish the whole scan if you
		 * abort it with stopScan!
		 */
		void stopScan(ReaderManagerPlugInType pType);

		QVector<ReaderManagerPlugInInfo> getPlugInInfos() const;
		QVector<ReaderInfo> getReaderInfos(ReaderManagerPlugInType pType) const;
		virtual QVector<ReaderInfo> getReaderInfos(const ReaderFilter& pFilter = ReaderFilter()) const;
		ReaderInfo getReaderInfo(const QString& pReaderName) const;

		/*!
		 * Executes a command to create a CardConnection for a specified reader.
		 * \param pReaderName The name of the reader.
		 * \param pReceiver The receiver object .
		 * \param pSlot The slot to receive CreateCardConnectionCommand.
		 */
		template<typename T>
		QMetaObject::Connection callCreateCardConnectionCommand(const QString& pReaderName, const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pSlot)
		{
			CreateCardConnectionCommand* command = new CreateCardConnectionCommand(pReaderName, mWorker);
			QMetaObject::Connection connection = connect(command, &CreateCardConnectionCommand::fireCommandDone, pReceiver, pSlot, Qt::UniqueConnection);
			if (connection)
			{
				QMetaObject::invokeMethod(command, "execute", Qt::QueuedConnection);
			}
			else
			{
				qCritical() << "Cannot invoke CreateCardConnectionCommand command";
				command->deleteLater();
			}

			return connection;
		}


		void connectReader(const QString& pReaderName);
		void disconnectReader(const QString& pReaderName);
		void disconnectAllReaders();

		QSharedPointer<RemoteClient> getRemoteClient();

	Q_SIGNALS:
		void firePluginAdded(const ReaderManagerPlugInInfo& pInfo);
		void fireStatusChanged(const ReaderManagerPlugInInfo& pInfo);
		void fireReaderAdded(const QString& pReaderName);
		void fireReaderRemoved(const QString& pReaderName);
		void fireReaderDeviceError(DeviceError pDeviceError);
		void fireReaderPropertiesUpdated(const QString& pReaderName);
		void fireCardInserted(const QString& pReaderName);
		void fireCardRemoved(const QString& pReaderName);
		void fireCardRetryCounterChanged(const QString& pReaderName);
		void fireReaderEvent();
		void fireInitialized();

	public Q_SLOTS:
		/*!
		 * Shuts down the reader manager service.
		 * The thread is terminated and the plug-ins are unloaded.
		 */
		void shutdown();
};

} /* namespace governikus */
