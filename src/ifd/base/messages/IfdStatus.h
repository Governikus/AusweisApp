/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessage.h"
#include "ReaderInfo.h"

#include <QJsonObject>


class test_RemoteIfdReaderManagerPlugIn;


namespace governikus
{
class MockIfdDispatcher;

class IfdStatus
	: public IfdMessage
{
	friend class ::test_RemoteIfdReaderManagerPlugIn;

	private:
		QString mSlotName;
		bool mHasPinPad;
		int mMaxApduLength;
		bool mConnectedReader;
		bool mCardAvailable;

		[[nodiscard]] QJsonValue createPaceCapabilities() const;
		void parsePinPad(const QJsonObject& pMessageObject);

	public:
		explicit IfdStatus(const ReaderInfo& pReaderInfo, bool pPublishCard = true);
		explicit IfdStatus(const QJsonObject& pMessageObject);
		~IfdStatus() override = default;

		[[nodiscard]] const QString& getSlotName() const;
		[[nodiscard]] bool hasPinPad() const;
		[[nodiscard]] int getMaxApduLength() const;
		[[nodiscard]] bool getConnectedReader() const;
		[[nodiscard]] bool getCardAvailable() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
