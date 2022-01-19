/*!
 * \brief Class that dispatches incoming and outgoing remote messages.
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteDispatcher.h"


namespace governikus
{
class RemoteDispatcherServer
	: public RemoteDispatcher
{
	Q_OBJECT

	private:
		void createAndSendContext(const QJsonObject& pMessageObject);
		bool processContext(RemoteCardMessageType pMsgType, const QJsonObject& pMsgObject) override;

	public:
		explicit RemoteDispatcherServer(const QSharedPointer<DataChannel>& pDataChannel);

	Q_SIGNALS:
		void fireContextEstablished();
};

} // namespace governikus
