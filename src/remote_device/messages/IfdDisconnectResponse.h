/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessageResponse.h"


namespace governikus
{
class IfdDisconnectResponse
	: public RemoteMessageResponse
{
	private:
		QString mSlotHandle;

	public:
		IfdDisconnectResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdDisconnectResponse(const QJsonObject& pMessageObject);
		~IfdDisconnectResponse() override = default;

		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] QByteArray toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
