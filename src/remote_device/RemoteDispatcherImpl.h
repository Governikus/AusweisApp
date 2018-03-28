/*!
 * \brief Class that dispatches incoming and outgoing remote messages.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DataChannel.h"
#include "messages/RemoteMessageParser.h"
#include "RemoteDispatcher.h"


namespace governikus
{
class RemoteDispatcherImpl
	: public RemoteDispatcher
{
	Q_OBJECT

	private:
		const QSharedPointer<DataChannel> mDataChannel;
		const RemoteMessageParser mParser;
		QString mContextHandle;

		void createAndSendContext(const QJsonObject& pMessageObject);
		void saveRemoteNameInSettings(const QString& pName);

	private Q_SLOTS:
		void onReceived(const QByteArray& pDataBlock);
		void onClosed(GlobalStatus::Code pCloseCode);

	public:
		RemoteDispatcherImpl(const QSharedPointer<DataChannel>& pDataChannel);
		virtual ~RemoteDispatcherImpl() override;

		virtual const QString& getId() const override;
		virtual const QString& getContextHandle() const override;
		Q_INVOKABLE virtual void close() override;
		Q_INVOKABLE virtual void send(const QSharedPointer<const RemoteMessage>& pMessage) override;
};

} /* namespace governikus */
