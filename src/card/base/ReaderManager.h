/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "ReaderFilter.h"
#include "ReaderManagerWorker.h"
#include "command/CreateCardConnectionCommand.h"
#include "command/ExecuteCommand.h"

#include <QMutex>
#include <QPointer>
#include <QThread>

#include <functional>


namespace governikus
{

class ReaderManager
	: public QObject
	, private Env::ThreadSafe
{
	Q_OBJECT
	friend class Env;

	private:
		static QVector<std::function<void()>> cMainThreadInit;

		mutable QMutex mMutex;
		QThread mThread;
		QPointer<ReaderManagerWorker> mWorker;
		QMap<QString, ReaderInfo> mReaderInfoCache;
		QMap<ReaderManagerPlugInType, ReaderManagerPlugInInfo> mPlugInInfoCache;

	protected:
		ReaderManager();
		~ReaderManager() override;

	public:
		static void addMainThreadInit(const std::function<void()>& pFunc);

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
		 * Insert a "virtual" card if plugin supports it.
		 */
		void insert(const ReaderInfo& pReaderInfo, const QVariant& pData = QVariant());

		/*!
		 * Shelve all cards that were shelved and inserted before.
		 */
		void shelve();

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

		ReaderManagerPlugInInfo getPlugInInfo(ReaderManagerPlugInType pType) const;
		virtual QVector<ReaderInfo> getReaderInfos(const ReaderFilter& pFilter = ReaderFilter()) const;
		ReaderInfo getReaderInfo(const QString& pReaderName) const;
		void updateReaderInfo(const QString& pReaderName);

		bool isWorkerThread() const
		{
			return QThread::currentThread() == &mThread;
		}


		/*!
		 * Executes a function on ReaderManager-Thread.
		 * \param pFunc Function that will be executed.
		 * \param pReceiver The receiver object.
		 * \param pSlot The slot to receive ExecuteCommand.
		 */
		template<typename T>
		QMetaObject::Connection callExecuteCommand(const std::function<QVariant()>& pFunc, const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pSlot)
		{
			const QMutexLocker mutexLocker(&mMutex);

			if (!mThread.isRunning())
			{
				qCWarning(card) << "Cannot call ExecuteCommand if ReaderManager-Thread is not active";
				return QMetaObject::Connection();
			}

			auto* command = new ExecuteCommand(pFunc);
			command->moveToThread(&mThread);
			QMetaObject::Connection connection = connect(command, &ExecuteCommand::fireCommandDone, pReceiver, pSlot, Qt::QueuedConnection);
			if (connection)
			{
				command->run();
			}
			else
			{
				qCCritical(card) << "Cannot invoke ExecuteCommand command";
				command->deleteLater();
			}

			return connection;
		}


#ifndef QT_NO_DEBUG

		/*!
		 * Executes a blocking function on ReaderManager-Thread.
		 * \param pFunc Function that will be executed.
		 */
		template<typename Func>
		void callExecute(Func pFunc)
		{
			if (!mThread.isRunning())
			{
				qCWarning(card) << "Cannot call Execute if ReaderManager-Thread is not active";
				return;
			}

			if (isWorkerThread())
			{
				pFunc();
				return;
			}

			const QMutexLocker mutexLocker(&mMutex);

			QMetaObject::invokeMethod(mWorker.data(), pFunc, Qt::BlockingQueuedConnection);
		}


#endif


		/*!
		 * Executes a command to create a CardConnection for a specified reader.
		 * \param pReaderName The name of the reader.
		 * \param pReceiver The receiver object.
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
		void fireCardInfoChanged(const ReaderInfo& pInfo);
		void fireInitialized();

	private Q_SLOTS:
		void doUpdateCacheEntry(const ReaderInfo& pInfo);
		void doRemoveCacheEntry(const ReaderInfo& pInfo);
		void doUpdatePluginCache(const ReaderManagerPlugInInfo& pInfo);

	public Q_SLOTS:
		/*!
		 * Shuts down the reader manager service.
		 * The thread is terminated and the plug-ins are unloaded.
		 */
		void shutdown();
};

} // namespace governikus
