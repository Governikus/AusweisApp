/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdClient.h"
#include "IfdDispatcherClient.h"
#include "ReaderManagerPlugin.h"
#include "messages/IfdStatus.h"

#include <QMap>
#include <QSharedPointer>


class test_RemoteIfdReaderManagerPlugin;


namespace governikus
{


class IfdReaderManagerPlugin
	: public ReaderManagerPlugin
{
	Q_OBJECT
	friend class ::test_RemoteIfdReaderManagerPlugin;

	private:
		QMultiMap<QString, QString> mReadersForDispatcher;
		QMap<QString, QSharedPointer<IfdDispatcherClient>> mDispatcherList;
		QMap<QString, Reader*> mReaderList;

		void processConnectedReader(const QString& pReaderName, const IfdStatus& pIfdStatus, const QSharedPointer<IfdDispatcherClient>& pDispatcher, const QString& pId);
		void handleIFDStatus(const QJsonObject& pJsonObject, const QString& pId);

	private Q_SLOTS:
		void onContextEstablished(const QString& pIfdName, const QString& pId) const;
		void onMessage(IfdMessageType pMessageType, const QJsonObject& pJsonObject, const QString& pId);
		void onDispatcherClosed(GlobalStatus::Code pCloseCode, const QString& pId);

	protected:
		virtual void addDispatcher(const QSharedPointer<IfdDispatcherClient>& pDispatcher);
		void removeAllDispatchers();
		void removeDispatcher(const QString& pId);
		[[nodiscard]] const QMap<QString, QSharedPointer<IfdDispatcherClient>>& getDispatchers() const;

		virtual IfdClient* getIfdClient() const = 0;

	public:
		IfdReaderManagerPlugin(ReaderManagerPluginType pPluginType, bool pAvailable = false, bool pPluginEnabled = false);
		~IfdReaderManagerPlugin() override;

		void init() override;
		[[nodiscard]] QList<Reader*> getReaders() const override;

		void insert(const QString& pReaderName, const QVariant& pData) override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;

};

} // namespace governikus
