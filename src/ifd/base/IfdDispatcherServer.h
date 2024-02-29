/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Class that dispatches incoming and outgoing ifd messages.
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
		void fireNameChanged();
};

} // namespace governikus
