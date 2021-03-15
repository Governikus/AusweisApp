/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderInfo.h"
#include "RemoteMessage.h"

#include <QJsonObject>


namespace governikus
{
class MockRemoteDispatcher;

class IfdStatus
	: public RemoteMessage
{
	private:
		QString mSlotName;
		bool mHasPinPad;
		int mMaxApduLength;
		bool mConnectedReader;
		bool mCardAvailable;

		[[nodiscard]] QJsonValue createPaceCapabilities() const;
		void parsePinPad(const QJsonObject& pMessageObject);

	public:
		explicit IfdStatus(const ReaderInfo& pReaderInfo);
		explicit IfdStatus(const QJsonObject& pMessageObject);
		~IfdStatus() override = default;

		[[nodiscard]] const QString& getSlotName() const;
		[[nodiscard]] bool hasPinPad() const;
		[[nodiscard]] int getMaxApduLength() const;
		[[nodiscard]] bool getConnectedReader() const;
		[[nodiscard]] bool getCardAvailable() const;
		[[nodiscard]] QByteArray toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
