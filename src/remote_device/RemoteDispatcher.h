/*!
 *
 * \brief An interface for RemoteHandleImpl, meant to omit the
 * dependency between card_base and remote_device.
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DataChannel.h"
#include "GlobalStatus.h"
#include "messages/IfdVersion.h"
#include "messages/RemoteMessage.h"

#include <QObject>
#include <QSharedPointer>


namespace governikus
{

class RemoteDispatcher
	: public QObject
{
	Q_OBJECT

	private:
		const QSharedPointer<DataChannel> mDataChannel;

		virtual bool processContext(RemoteCardMessageType pMsgType, const QJsonObject& pMsgObject) = 0;

	private Q_SLOTS:
		void onReceived(const QByteArray& pDataBlock);
		void onClosed(GlobalStatus::Code pCloseCode);

	protected:
		const IfdVersion::Version mVersion;
		QString mContextHandle;

	public:
		explicit RemoteDispatcher(IfdVersion::Version pVersion, const QSharedPointer<DataChannel>& pDataChannel = QSharedPointer<DataChannel>());
		~RemoteDispatcher();

		virtual QString getId() const;
		virtual const QString& getContextHandle() const;
		void saveRemoteNameInSettings(const QString& pName);

		void close();
		Q_INVOKABLE virtual void send(const QSharedPointer<const RemoteMessage>& pMessage);

	Q_SIGNALS:
		void fireReceived(RemoteCardMessageType pMessageType, const QJsonObject& pJsonObject, const QString& pId);
		void fireClosed(GlobalStatus::Code pCloseCode, const QString& pId);
};

} // namespace governikus
