/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardReturnCode.h"
#include "RemoteMessageResponse.h"


namespace governikus
{
class IfdModifyPinResponse
	: public RemoteMessageResponse
{
	private:
		QString mSlotHandle;
		QByteArray mOutputData;

	public:
		IfdModifyPinResponse(const QString& pSlotHandle, const QByteArray& pOutputData, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdModifyPinResponse(const QJsonObject& pMessageObject);
		~IfdModifyPinResponse() override = default;

		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] const QByteArray& getOutputData() const;
		[[nodiscard]] CardReturnCode getReturnCode() const;

		[[nodiscard]] QByteArray toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
