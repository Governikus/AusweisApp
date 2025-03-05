/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ECardApiResult.h"
#include "IfdMessage.h"


namespace governikus
{
class IfdMessageResponse
	: public IfdMessage
{
	private:
		ECardApiResult::Major mResultMajor;
		ECardApiResult::Minor mResultMinor;

	protected:
		[[nodiscard]] QJsonObject createMessageBody(const QString& pContextHandle) const override;

	public:
		IfdMessageResponse(IfdMessageType pType, ECardApiResult::Minor pResultMinor);
		explicit IfdMessageResponse(const QJsonObject& pMessageObject);
		~IfdMessageResponse() override = default;

		[[nodiscard]] bool resultHasError() const;
		[[nodiscard]] ECardApiResult::Minor getResultMinor() const;
};


} // namespace governikus
