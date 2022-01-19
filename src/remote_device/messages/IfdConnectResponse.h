/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessageResponse.h"


namespace governikus
{
class IfdConnectResponse
	: public RemoteMessageResponse
{
	private:
		QString mSlotHandle;
		QString mError;

	public:
		IfdConnectResponse(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdConnectResponse(const QJsonObject& pMessageObject);
		~IfdConnectResponse() override = default;

		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] QByteArray toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
