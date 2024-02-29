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

class IfdDispatcherClient
	: public IfdDispatcher
{
	Q_OBJECT

	private:
		bool processContext(IfdMessageType pMsgType, const QJsonObject& pMsgObject) override;

	public:
		IfdDispatcherClient(IfdVersion::Version pVersion, const QSharedPointer<DataChannel>& pDataChannel);

		Q_INVOKABLE virtual void sendEstablishContext();

	Q_SIGNALS:
		void fireContextEstablished(const QString& pIfdName, const QString& pId);
};

} // namespace governikus
