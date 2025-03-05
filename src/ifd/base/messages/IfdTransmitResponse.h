/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessageResponse.h"

#include <QByteArray>


namespace governikus
{
class IfdTransmitResponse
	: public IfdMessageResponse
{
	private:
		QString mSlotHandle;
		QByteArray mResponseApdu;

		void parseResponseApdu(const QJsonObject& pMessageObject);

	public:
		IfdTransmitResponse(const QString& pSlotHandle, const QByteArray& pResponseApdu = QByteArray(), ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdTransmitResponse(const QJsonObject& pMessageObject);
		~IfdTransmitResponse() override = default;

		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] const QByteArray& getResponseApdu() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
