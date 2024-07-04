/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Worker implementation of ReaderManger thread
 */

#pragma once

#include "CardConnectionWorker.h"
#include "ReaderInfo.h"
#include "ReaderManagerPlugin.h"
#include "ReaderManagerPluginInfo.h"

#include <QObject>

namespace governikus
{
class ReaderManagerWorker
	: public QObject
{
	Q_OBJECT

	private:
		QList<ReaderManagerPlugin*> mPlugins;

		void callOnPlugin(ReaderManagerPluginType pType, const std::function<void(ReaderManagerPlugin* pPlugin)>& pFunc, const char* pLog);
		void registerPlugins();
		[[nodiscard]] static bool isPlugin(const QJsonObject& pJson);
		void registerPlugin(ReaderManagerPlugin* pPlugin);
		[[nodiscard]] Reader* getReader(const QString& pReaderName) const;

	public:
		ReaderManagerWorker();
		~ReaderManagerWorker() override;

		Q_INVOKABLE void shutdown();

		Q_INVOKABLE void reset(ReaderManagerPluginType pType);
		Q_INVOKABLE void insert(const ReaderInfo& pReaderInfo, const QVariant& pData);
		Q_INVOKABLE void shelve();
		Q_INVOKABLE void startScan(ReaderManagerPluginType pType, bool pAutoConnect);
		Q_INVOKABLE void stopScan(ReaderManagerPluginType pType, const QString& pError);

		[[nodiscard]] Q_INVOKABLE QList<ReaderInfo> getReaderInfos() const;
		Q_INVOKABLE void updateReaderInfo(const QString& pReaderName);
		void createCardConnectionWorker(const QString& pReaderName, const std::function<QSharedPointer<CardConnectionWorker>(const QSharedPointer<CardConnectionWorker>&)>& pInitWorker);

	Q_SIGNALS:
		void firePluginAdded(const ReaderManagerPluginInfo& pInfo);
		void fireStatusChanged(const ReaderManagerPluginInfo& pInfo);
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
