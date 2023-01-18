/*!
 * \brief Class that dispatches incoming and outgoing ifd messages.
 *
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
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
