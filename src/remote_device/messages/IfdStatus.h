/*!
 * \brief Classes that model the IFDStatus message.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderInfo.h"
#include "RemoteMessage.h"

#include <QJsonObject>


namespace governikus
{
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
		QString mSlotName;
		PaceCapabilities mPaceCapabilities;
		int mMaxApduLength;
		bool mConnectedReader;
		bool mCardAvailable;

	public:
		IfdStatus(const ReaderInfo& pReaderInfo);
		IfdStatus(const QJsonObject& pMessageObject);
		virtual ~IfdStatus() override = default;

		const QString& getSlotName() const;
		const PaceCapabilities& getPaceCapabilities() const;
		int getMaxApduLength() const;
		bool getConnectedReader() const;
		bool getCardAvailable() const;
		virtual QJsonDocument toJson(const QString& pContextHandle) const override;
};


} /* namespace governikus */
