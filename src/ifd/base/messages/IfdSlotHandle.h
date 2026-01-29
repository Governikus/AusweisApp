/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ECardApiResult.h"
#include "IfdMessage.h"

#include <QJsonObject>
#include <QString>


namespace governikus
{
template<typename T> class IfdSlotHandle
	: public T
{
	private:
		QString mSlotHandle;
		static constexpr QLatin1String cSlotHandleName = QLatin1String("SlotHandle");

	protected:
		IfdSlotHandle(IfdMessageType pType, const QString& pSlotHandle)
			: T(pType)
			, mSlotHandle(pSlotHandle)
		{
		}


		IfdSlotHandle(IfdMessageType pType, const QString& pSlotHandle, ECardApiResult::Minor pResultMinor)
			: T(pType, pResultMinor)
			, mSlotHandle(pSlotHandle)
		{
		}


		explicit IfdSlotHandle(const QJsonObject& pMessageObject)
			: T(pMessageObject)
			, mSlotHandle()
		{
			mSlotHandle = T::getStringValue(pMessageObject, cSlotHandleName);
		}


		[[nodiscard]] QJsonObject createMessageBody(const QString& pContextHandle) const override
		{
			QJsonObject messageBody = T::createMessageBody(pContextHandle);
			messageBody[cSlotHandleName] = mSlotHandle;
			return messageBody;
		}

	public:
		[[nodiscard]] const QString& getSlotHandle() const
		{
			return mSlotHandle;
		}


};

} // namespace governikus
