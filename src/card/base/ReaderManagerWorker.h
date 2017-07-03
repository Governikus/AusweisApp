/*!
 * \brief Worker implementation of ReaderManger thread
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "CardConnectionWorker.h"
#include "ReaderInfo.h"
#include "ReaderManagerPlugIn.h"
#include "ReaderManagerPlugInInfo.h"

#include <QObject>

namespace governikus
{

class ReaderManagerWorker
	: public QObject
{
	Q_OBJECT

	private:
		QVector<ReaderManagerPlugIn*> mPlugIns;
		bool mScanStarted;

		void registerPlugIns();
		bool isPlugIn(const QJsonObject& pJson);
		void registerPlugIn(ReaderManagerPlugIn* pPlugIn);
		Reader* getReader(const QString& pReaderName) const;

	public:
		ReaderManagerWorker();
		~ReaderManagerWorker();

		Q_INVOKABLE void startScan();
		Q_INVOKABLE void stopScan();

		Q_INVOKABLE QVector<ReaderManagerPlugInInfo> getPlugInInfos() const;
		Q_INVOKABLE QVector<ReaderInfo> getReaderInfos(const QVector<ReaderManagerPlugInType>& pTypes) const;
		Q_INVOKABLE ReaderInfo getReaderInfo(const QString& pReaderName) const;
		Q_INVOKABLE void createCardConnectionWorker(const QString& pReaderName);
		Q_INVOKABLE void connectReader(const QString& pReaderName);
		Q_INVOKABLE void disconnectReader(const QString& pReaderName);
		Q_INVOKABLE void disconnectAllReaders();

	Q_SIGNALS:
		void fireStatusChanged(const ReaderManagerPlugInInfo& pInfo);
		void fireReaderAdded(const QString& pReaderName);
		void fireReaderConnected(const QString& pReaderName);
		void fireReaderRemoved(const QString& pReaderName);
		void fireReaderPropertiesUpdated();
		void fireCardInserted(const QString& pReaderName);
		void fireCardRemoved(const QString& pReaderName);
		void fireCardRetryCounterChanged(const QString& pReaderName);
		void fireCardConnectionWorkerCreated(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

	public Q_SLOTS:
		void onThreadStarted();
};

} /* namespace governikus */
