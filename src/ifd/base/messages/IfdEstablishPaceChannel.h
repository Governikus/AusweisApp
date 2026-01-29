/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessage.h"
#include "IfdSlotHandle.h"
#include "pinpad/EstablishPaceChannel.h"


namespace governikus
{

class IfdEstablishPaceChannel
	: public IfdSlotHandle<IfdMessage>
{
	private:
		EstablishPaceChannel mInputData;
		int mExpectedPinLength;

		void parseInputData(const QJsonObject& pMessageObject);

	public:
		IfdEstablishPaceChannel(const QString& pSlotHandle, const EstablishPaceChannel& pInputData, int pExpectedPinLength);
		explicit IfdEstablishPaceChannel(const QJsonObject& pMessageObject);
		~IfdEstablishPaceChannel() override = default;

		[[nodiscard]] const EstablishPaceChannel& getInputData() const;
		[[nodiscard]] int getExpectedPinLength() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
