/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
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
		[[nodiscard]] QJsonObject createMessageBody(const QString& pContextHandle) const override;

	public:
		RemoteMessageResponse(RemoteCardMessageType pType, ECardApiResult::Minor pResultMinor);
		explicit RemoteMessageResponse(const QJsonObject& pMessageObject);
		~RemoteMessageResponse() override = default;

		[[nodiscard]] bool resultHasError() const;
		[[nodiscard]] ECardApiResult::Minor getResultMinor() const;
};


} // namespace governikus
