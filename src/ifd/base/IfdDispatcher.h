/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DataChannel.h"
#include "GlobalStatus.h"
#include "messages/IfdMessage.h"
#include "messages/IfdVersion.h"

#include <QObject>
#include <QSharedPointer>


namespace governikus
{

class IfdDispatcher
	: public QObject
{
	Q_OBJECT

	private:
		const QSharedPointer<DataChannel> mDataChannel;
		IfdVersion::Version mVersion;
		QString mContextHandle;

		virtual bool processContext(IfdMessageType pMsgType, const QJsonObject& pMsgObject) = 0;

	private Q_SLOTS:
		void onReceived(const QByteArray& pDataBlock);
		void onClosed(GlobalStatus::Code pCloseCode);

	protected:
		void setContextHandle(const QString& pHandle);
		void setVersion(IfdVersion::Version pVersion);

	public:
		explicit IfdDispatcher(IfdVersion::Version pVersion, const QSharedPointer<DataChannel>& pDataChannel);
		~IfdDispatcher() override;

		[[nodiscard]] virtual bool isPairingConnection() const;
		[[nodiscard]] virtual QByteArray getId() const;
		[[nodiscard]] virtual const QString& getContextHandle() const;
		[[nodiscard]] IfdVersion::Version getVersion() const;
		void saveRemoteNameInSettings(const QString& pName) const;

		void close();
		Q_INVOKABLE virtual void send(const QSharedPointer<const IfdMessage>& pMessage);

	Q_SIGNALS:
		void fireReceived(IfdMessageType pMessageType, const QJsonObject& pJsonObject, const QByteArray& pId);
		void fireClosed(GlobalStatus::Code pCloseCode, const QByteArray& pId);
};

} // namespace governikus
