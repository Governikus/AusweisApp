/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessageResponse.h"

#include <QByteArray>


namespace governikus
{
class IfdTransmitResponse
	: public RemoteMessageResponse
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
		[[nodiscard]] QByteArray toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
