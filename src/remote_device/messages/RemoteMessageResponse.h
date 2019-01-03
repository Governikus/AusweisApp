/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ECardApiResult.h"
#include "RemoteMessage.h"


namespace governikus
{
class RemoteMessageResponse
	: public RemoteMessage
{
	private:
		ECardApiResult::Major mResultMajor;
		ECardApiResult::Minor mResultMinor;

	protected:
		virtual QJsonObject createMessageBody(const QString& pContextHandle) const override;

	public:
		RemoteMessageResponse(RemoteCardMessageType pType, ECardApiResult::Minor pResultMinor);
		RemoteMessageResponse(const QJsonObject& pMessageObject);
		virtual ~RemoteMessageResponse() override = default;

		bool resultHasError() const;
		ECardApiResult::Minor getResultMinor() const;
};


} // namespace governikus
