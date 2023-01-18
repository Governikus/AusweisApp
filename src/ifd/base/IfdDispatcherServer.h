/*!
 * \brief Class that dispatches incoming and outgoing ifd messages.
 *
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdDispatcher.h"


namespace governikus
{
class IfdDispatcherServer
	: public IfdDispatcher
{
	Q_OBJECT

	private:
		void createAndSendContext(const QJsonObject& pMessageObject);
		bool processContext(IfdMessageType pMsgType, const QJsonObject& pMsgObject) override;

	public:
		explicit IfdDispatcherServer(const QSharedPointer<DataChannel>& pDataChannel);

	Q_SIGNALS:
		void fireContextEstablished();
};

} // namespace governikus
