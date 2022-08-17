/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "IfdVersion.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>


#define VALUE_NAME(_name, _key)\
	inline QLatin1String _name(){\
		return QLatin1String(_key);\
	}


namespace governikus
{
defineEnumType(IfdMessageType,
		IFDEstablishContext,
		IFDEstablishContextResponse,
		IFDGetStatus,
		IFDStatus,
		IFDConnect,
		IFDConnectResponse,
		IFDDisconnect,
		IFDDisconnectResponse,
		IFDError,
		IFDTransmit,
		IFDTransmitResponse,
		IFDEstablishPACEChannel,
		IFDEstablishPACEChannelResponse,
		IFDModifyPIN,
		IFDModifyPINResponse,
		UNDEFINED)


class IfdMessage
{
	private:
		bool mIncomplete;
		IfdMessageType mMessageType;
		QString mContextHandle;

	protected:
		[[nodiscard]] virtual QJsonObject createMessageBody(const QString& pContextHandle) const;
		static QByteArray toByteArray(const QJsonObject& pJsonObject);

		void markIncomplete(const QString& pLogMessage);
		void missingValue(const QLatin1String& pName);
		void invalidType(const QLatin1String& pName, const QLatin1String& pExpectedType);
		bool getBoolValue(const QJsonObject& pJsonObject, const QLatin1String& pName);
		int getIntValue(const QJsonObject& pJsonObject, const QLatin1String& pName, int pDefault);
		QString getStringValue(const QJsonObject& pJsonObject, const QLatin1String& pName);

	public:
		static QJsonObject parseByteArray(const QByteArray& pMessage);

		explicit IfdMessage(IfdMessageType pType);
		explicit IfdMessage(const QJsonObject& pMessageObject);
		virtual ~IfdMessage() = default;

		[[nodiscard]] bool isIncomplete() const;
		[[nodiscard]] IfdMessageType getType() const;
		[[nodiscard]] const QString& getContextHandle() const;

		[[nodiscard]] virtual QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle = QString()) const;
};


} // namespace governikus
