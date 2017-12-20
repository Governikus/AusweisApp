/*!
 * \brief Classes that model remote card reader messages.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>


#define VALUE_NAME(_name, _key)\
	inline QLatin1String _name(){\
		return QLatin1String(_key);\
	}


namespace governikus
{
defineEnumType(RemoteCardMessageType,
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
		UNDEFINED)


class RemoteMessage
{
	private:
		bool mIsValid;
		RemoteCardMessageType mMessageType;
		QString mContextHandle;

	protected:
		virtual QJsonObject createMessageBody(const QString& pContextHandle) const;
		void missingValue(const QLatin1String& pName);
		void invalidType(const QLatin1String& pName, const QLatin1String& pExpectedType);
		bool getBoolValue(const QJsonObject& pJsonObject, const QLatin1String& pName);
		int getIntValue(const QJsonObject& pJsonObject, const QLatin1String& pName);
		QString getStringValue(const QJsonObject& pJsonObject, const QLatin1String& pName);

	public:
		static QJsonObject parseByteArray(const QByteArray& pMessage);

		RemoteMessage(RemoteCardMessageType pType);
		RemoteMessage(const QJsonObject& pMessageObject);
		virtual ~RemoteMessage() = default;

		bool isValid() const;
		RemoteCardMessageType getType() const;
		const QString& getContextHandle() const;

		virtual QJsonDocument toJson(const QString& pContextHandle) const;
};


} /* namespace governikus */
