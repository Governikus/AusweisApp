/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "command/CreateCardConnectionCommand.h"
#include "Env.h"
#include "Reader.h"
#include "ReaderFilter.h"
#include "ReaderManagerWorker.h"

#include <QMutex>
#include <QPointer>
#include <QThread>


namespace governikus
{
class ReaderManager
	: public QObject
	, private Env::ThreadSafe
{
	Q_OBJECT
	friend class Env;

	private:
		mutable QMutex mMutex;
		QThread mThread;
		QPointer<ReaderManagerWorker> mWorker;
		QMap<QString, ReaderInfo> mReaderInfoCache;
		QMap<ReaderManagerPlugInType, ReaderManagerPlugInInfo> mPlugInInfoCache;

	protected:
		ReaderManager();
		~ReaderManager() override;

	public:
		/*!
		 * Initialize the reader manager service.
		 * The thread is started and the plug-ins are initialized, too.
		 */
		void init();

		/*!
		 * Resets the plugins matching the given type.
		 */
		void reset(ReaderManagerPlugInType pType);

		/*!
		 * Starts a scan for all device types.
		 */
		void startScanAll(bool pAutoConnect = true);

		/*!
		 * Starts a scan for devices if registered plugin don't scan anytime.
		 */
		void startScan(ReaderManagerPlugInType pType, bool pAutoConnect = true);

		/*!
		 * Stops scan for all device types.
		 */
		void stopScanAll(const QString& pError = QString());

		/*!
		 * Queries if any plugin is currently scanning
		 */
		bool isScanRunning() const;

		/*!
		 * Queries if a plugin with the requested type is currently scanning
		 */
		bool isScanRunning(ReaderManagerPlugInType pType) const;

		/*!
		 * Stops started scan for devices.
		 * Be aware that some plugins don't finish the whole scan if you
		 * abort it with stopScan!
		 */
		void stopScan(ReaderManagerPlugInType pType, const QString& pError = QString());

		QVector<ReaderManagerPlugInInfo> getPlugInInfos() const;
		virtual QVector<ReaderInfo> getReaderInfos(const ReaderFilter& pFilter = ReaderFilter()) const;
		ReaderInfo getReaderInfo(const QString& pReaderName) const;
		void updateReaderInfo(const QString& pReaderName);

		/*!
		 * Executes a command to create a CardConnection for a specified reader.
		 * \param pReaderName The name of the reader.
		 * \param pReceiver The receiver object .
		 * \param pSlot The slot to receive CreateCardConnectionCommand.
		 */
		template<typename T>
		QMetaObject::Connection callCreateCardConnectionCommand(const QString& pReaderName, const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pSlot)
		{
			auto* command = new CreateCardConnectionCommand(pReaderName, mWorker);
			QMetaObject::Connection connection = connect(command, &CreateCardConnectionCommand::fireCommandDone, pReceiver, pSlot, Qt::UniqueConnection);
			if (connection)
			{
				command->run();
			}
			else
			{
				qCCritical(card) << "Cannot invoke CreateCardConnectionCommand command";
				command->deleteLater();
			}

			return connection;
		}


		void updateRetryCounters();

	Q_SIGNALS:
		void firePluginAdded(const ReaderManagerPlugInInfo& pInfo);
		void fireStatusChanged(const ReaderManagerPlugInInfo& pInfo);
		void fireReaderAdded(const ReaderInfo& pInfo);
		void fireReaderRemoved(const ReaderInfo& pInfo);
		void fireReaderPropertiesUpdated(const ReaderInfo& pInfo);
		void fireCardInserted(const ReaderInfo& pInfo);
		void fireCardRemoved(const ReaderInfo& pInfo);
		void fireCardRetryCounterChanged(const ReaderInfo& pInfo);
		void fireReaderEvent();
		void fireInitialized();

	private Q_SLOTS:
		void doUpdateCacheEntry(const ReaderInfo& pInfo);
		void doRemoveCacheEntry(const ReaderInfo& pInfo);
		void doFullUpdateCache(const ReaderManagerPlugInInfo& pInfo);

	public Q_SLOTS:
		/*!
		 * Shuts down the reader manager service.
		 * The thread is terminated and the plug-ins are unloaded.
		 */
		void shutdown();
};

} // namespace governikus
