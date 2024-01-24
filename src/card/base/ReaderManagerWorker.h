/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Worker implementation of ReaderManger thread
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

		void callOnPlugIn(ReaderManagerPlugInType pType, const std::function<void(ReaderManagerPlugIn* pPlugIn)>& pFunc, const char* pLog);
		void registerPlugIns();
		[[nodiscard]] static bool isPlugIn(const QJsonObject& pJson);
		void registerPlugIn(ReaderManagerPlugIn* pPlugIn);
		[[nodiscard]] Reader* getReader(const QString& pReaderName) const;

	public:
		ReaderManagerWorker();
		~ReaderManagerWorker() override;

		Q_INVOKABLE void shutdown();

		Q_INVOKABLE void reset(ReaderManagerPlugInType pType);
		Q_INVOKABLE void insert(const ReaderInfo& pReaderInfo, const QVariant& pData);
		Q_INVOKABLE void shelve();
		Q_INVOKABLE void startScan(ReaderManagerPlugInType pType, bool pAutoConnect);
		Q_INVOKABLE void stopScan(ReaderManagerPlugInType pType, const QString& pError);

		[[nodiscard]] Q_INVOKABLE QVector<ReaderInfo> getReaderInfos() const;
		Q_INVOKABLE void updateReaderInfo(const QString& pReaderName);
		Q_INVOKABLE void createCardConnectionWorker(const QString& pReaderName);

	Q_SIGNALS:
		void firePluginAdded(const ReaderManagerPlugInInfo& pInfo);
		void fireStatusChanged(const ReaderManagerPlugInInfo& pInfo);
		void fireReaderAdded(const ReaderInfo& pInfo);
		void fireReaderRemoved(const ReaderInfo& pInfo);
		void fireReaderPropertiesUpdated(const ReaderInfo& pInfo);
		void fireCardInserted(const ReaderInfo& pInfo);
		void fireCardRemoved(const ReaderInfo& pInfo);
		void fireCardInfoChanged(const ReaderInfo& pInfo);
		void fireCardConnectionWorkerCreated(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);
		void fireInitialized();

	private Q_SLOTS:
		void onReaderRemoved(ReaderInfo pInfo);

	public Q_SLOTS:
		void onThreadStarted();
};

} // namespace governikus
