/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessageResponse.h"
#include "IfdSlotHandle.h"


namespace governikus
{
class IfdTransmitResponse
	: public IfdSlotHandle<IfdMessageResponse>
{
	private:
		QByteArray mResponseApdu;

		void parseResponseApdu(const QJsonObject& pMessageObject);

	public:
		explicit IfdTransmitResponse(const QString& pSlotHandle, const QByteArray& pResponseApdu = QByteArray(), ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdTransmitResponse(const QJsonObject& pMessageObject);
		~IfdTransmitResponse() override = default;

		[[nodiscard]] const QByteArray& getResponseApdu() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
