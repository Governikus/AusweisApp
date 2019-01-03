/*!
 * \brief Class that dispatches incoming and outgoing remote messages.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteDispatcher.h"


namespace governikus
{

class RemoteDispatcherClient
	: public RemoteDispatcher
{
	Q_OBJECT

	private:
		virtual bool processContext(RemoteCardMessageType pMsgType, const QJsonObject& pMsgObject) override;

	public:
		RemoteDispatcherClient(IfdVersion::Version pVersion, const QSharedPointer<DataChannel>& pDataChannel);

		Q_INVOKABLE virtual void sendEstablishContext();

	Q_SIGNALS:
		void fireContextEstablished(const QString& pIfdName, const QString& pId);
};

} // namespace governikus
