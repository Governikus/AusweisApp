/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdClient.h"
#include "IfdDispatcherClient.h"
#include "IfdReader.h"
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
		QMultiMap<QByteArray, QString> mReadersForDispatcher;
		QMap<QByteArray, QSharedPointer<IfdDispatcherClient>> mDispatcherList;
		QMap<QString, QSharedPointer<IfdReader>> mReaders;

		void processConnectedReader(const QString& pReaderName, const IfdStatus& pIfdStatus, const QSharedPointer<IfdDispatcherClient>& pDispatcher, const QByteArray& pId);
		void handleIFDStatus(const QJsonObject& pJsonObject, const QByteArray& pId);

	private Q_SLOTS:
		void onContextEstablished(const QString& pIfdName, const QByteArray& pId) const;
		void onMessage(IfdMessageType pMessageType, const QJsonObject& pJsonObject, const QByteArray& pId);
		void onDispatcherClosed(GlobalStatus::Code pCloseCode, const QByteArray& pId);

	protected:
		virtual void addDispatcher(const QSharedPointer<IfdDispatcherClient>& pDispatcher);
		void removeAllDispatchers();
		void removeDispatcher(const QByteArray& pId);
		[[nodiscard]] const QMap<QByteArray, QSharedPointer<IfdDispatcherClient>>& getDispatchers() const;

		virtual IfdClient* getIfdClient() const = 0;

	public:
		explicit IfdReaderManagerPlugin(ReaderManagerPluginType pPluginType, bool pAvailable = false, bool pPluginEnabled = false);
		~IfdReaderManagerPlugin() override;

		void init() override;

		[[nodiscard]] QPointer<Reader> getReader(const QString& pReaderName) const override;

		void insert(const QString& pReaderName, const QVariant& pData) override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;

		void shelveAll() const override;
};

} // namespace governikus
