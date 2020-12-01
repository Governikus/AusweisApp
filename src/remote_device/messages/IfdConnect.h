/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessage.h"


namespace governikus
{
class IfdConnect
	: public RemoteMessage
{
	private:
		QString mSlotName;
		bool mExclusive;

	public:
		IfdConnect(const QString& pSlotName, bool pExclusive = true);
		explicit IfdConnect(const QJsonObject& pMessageObject);
		virtual ~IfdConnect() override = default;

		const QString& getSlotName() const;
		bool isExclusive() const;
		virtual QByteArray toByteArray(const QString& pContextHandle) const override;
};


} // namespace governikus
