/*!
 * \brief Base of all messages of JSON API.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "MsgTypes.h"

#include <QJsonObject>

namespace governikus
{
class MsgHandler
{
	private:
		const MsgType mType;
		bool mVoid;

		MsgHandler();

	protected:
		QJsonObject mJsonObject;

		MsgHandler(MsgType pType);
		MsgHandler(MsgType pType, const char* pKey, const QString& pValue);
		MsgHandler(MsgType pType, const char* pKey, const QLatin1String& pValue);

		void setValue(const QLatin1String& pKey, const QString& pValue);
		void setValue(const char* pKey, const QString& pValue);
		void setValue(const QLatin1String& pKey, const QLatin1String& pValue);
		void setValue(const char* pKey, const QLatin1String& pValue);

		void setVoid(bool pVoid = true);

	public:
		static const MsgHandler Void;
		static const MsgLevel DEFAULT_MSG_LEVEL;
		static MsgType getStateMsgType(const QString& pState);

		QByteArray toJson() const;
		QByteArray getOutput() const;
		bool isVoid() const;
		MsgType getType() const;

		void setRequest(const QJsonObject& pRequest);
};

inline QDebug operator<<(QDebug pDbg, const MsgHandler& pMsg)
{
	QDebugStateSaver saver(pDbg);
	pDbg << pMsg.getType();
	return pDbg.space();
}


} /* namespace governikus */
