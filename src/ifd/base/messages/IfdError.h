/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessageResponse.h"


namespace governikus
{
class IfdError
	: public IfdMessageResponse
{
	private:
		QString mSlotHandle;

	public:
		IfdError(const QString& pSlotHandle, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdError(const QJsonObject& pMessageObject);
		~IfdError() override = default;

		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
