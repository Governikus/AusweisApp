/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "command/CreateCardConnectionCommand.h"
#include "Reader.h"
#include "ReaderManagerWorker.h"

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

	protected:
		ReaderManager();
		~ReaderManager();

	public:
		static ReaderManager& getInstance();

		/*!
		 * Initialize the reader manager service.
		 * The thread is started and the plug-ins are initialized, too.
		 */
		void init();

		/*!
		 * Starts a scan for devices if registered plugin don't scan anytime.
		 */
		void startScan();

		/*!
		 * Stops started scan for devices.
		 * Be aware that some plugins doesn't finished the whole scan if you
		 * abort it with stopScan!
		 */
		void stopScan();

		QVector<ReaderManagerPlugInInfo> getPlugInInfos() const;
		QVector<ReaderInfo> getReaderInfos(ReaderManagerPlugInType pType) const;
		QVector<ReaderInfo> getReaderInfos(const QVector<ReaderManagerPlugInType>& pTypes = EnumReaderManagerPlugInType::getList()) const;
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

	Q_SIGNALS:
		void fireStatusChanged(const ReaderManagerPlugInInfo& pInfo);
		void fireReaderAdded(const QString& pReaderName);
		void fireReaderConnected(const QString& pReaderName);
		void fireReaderRemoved(const QString& pReaderName);
		void fireReaderPropertiesUpdated();
		void fireCardInserted(const QString& pReaderName);
		void fireCardRemoved(const QString& pReaderName);
		void fireCardRetryCounterChanged(const QString& pReaderName);
		void fireReaderEvent();

	public Q_SLOTS:
		/*!
		 * Shuts down the reader manager service.
		 * The thread is terminated and the plug-ins are unloaded.
		 */
		void shutdown();
};

} /* namespace governikus */
