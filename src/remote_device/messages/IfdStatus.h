/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderInfo.h"
#include "RemoteMessage.h"

#include <QJsonObject>


class test_RemoteReaderManagerPlugIn;
class test_ServerMessageHandler;
class test_IfdStatus;


namespace governikus
{
class MockRemoteDispatcher;


class PaceCapabilities
{
	// PACECapabilities according to TR-03119, sec. D.1.1.

	private:
		bool mPace;
		bool mEId;
		bool mESign;
		bool mDestroy;

	public:
		PaceCapabilities(bool pPace = false, bool pEId = false, bool pESign = false, bool pDestroy = false);

		bool getPace() const;
		bool getEId() const;
		bool getESign() const;
		bool getDestroy() const;

		QJsonValue toJson() const;
};


class IfdStatus
	: public RemoteMessage
{
	private:
		friend MockRemoteDispatcher;
		friend ::test_IfdStatus;

		QString mSlotName;
		PaceCapabilities mPaceCapabilities;
		int mMaxApduLength;
		bool mConnectedReader;
		bool mCardAvailable;

		IfdStatus(const QString& pSlotName,
				const PaceCapabilities& pPaceCapabilities,
				int pMaxApduLength,
				bool pConnected,
				bool pCardAvailable = false);

	public:
		explicit IfdStatus(const ReaderInfo& pReaderInfo);
		explicit IfdStatus(const QJsonObject& pMessageObject);
		virtual ~IfdStatus() override = default;

		const QString& getSlotName() const;
		const PaceCapabilities& getPaceCapabilities() const;
		int getMaxApduLength() const;
		bool getConnectedReader() const;
		bool getCardAvailable() const;
		virtual QByteArray toByteArray(const QString& pContextHandle) const override;
};


} // namespace governikus
