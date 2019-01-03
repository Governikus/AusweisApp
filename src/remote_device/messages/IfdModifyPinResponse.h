/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
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
		IfdModifyPinResponse(const QJsonObject& pMessageObject);
		virtual ~IfdModifyPinResponse() override = default;

		const QString& getSlotHandle() const;
		const QByteArray& getOutputData() const;
		CardReturnCode getReturnCode() const;

		virtual QByteArray toByteArray(const QString& pContextHandle) const override;
};


} // namespace governikus
