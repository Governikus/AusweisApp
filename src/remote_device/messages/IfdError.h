/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessageResponse.h"


namespace governikus
{
class IfdError
	: public RemoteMessageResponse
{
	private:
		QString mSlotHandle;

	public:
		IfdError(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		IfdError(const QJsonObject& pMessageObject);
		virtual ~IfdError() override = default;

		const QString& getSlotHandle() const;
		virtual QByteArray toByteArray(const QString& pContextHandle) const override;
};


} // namespace governikus
