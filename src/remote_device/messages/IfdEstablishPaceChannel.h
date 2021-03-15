/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EstablishPaceChannel.h"
#include "RemoteMessage.h"


namespace governikus
{

class IfdEstablishPaceChannel
	: public RemoteMessage
{
	private:
		QString mSlotHandle;
		EstablishPaceChannel mInputData;
		int mPreferredPinLength;

		void parseInputData(const QJsonObject& pMessageObject);

	public:
		IfdEstablishPaceChannel(const QString& pSlotHandle, const EstablishPaceChannel& pInputData, int pPreferredPinLength);
		explicit IfdEstablishPaceChannel(const QJsonObject& pMessageObject);
		~IfdEstablishPaceChannel() override = default;

		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] const EstablishPaceChannel& getInputData() const;
		[[nodiscard]] int getPreferredPinLength() const;
		[[nodiscard]] QByteArray toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
