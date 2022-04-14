/*!
 * \brief Base of all messages of JSON API.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Msg.h"
#include "MsgTypes.h"
#include "SmartCardDefinitions.h"

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

		explicit MsgHandler(MsgType pType);
		explicit MsgHandler(MsgType pType, const char* const pKey, const QString& pValue);
		explicit MsgHandler(MsgType pType, const char* const pKey, const QLatin1String pValue);

		void setValue(const QLatin1String pKey, const QString& pValue);
		void setValue(const char* const pKey, const QString& pValue);
		void setValue(const QLatin1String pKey, const QLatin1String pValue);
		void setValue(const char* const pKey, const QLatin1String pValue);

		void setVoid(bool pVoid = true);

	public:
		static const MsgHandler Void;
		static const MsgLevel DEFAULT_MSG_LEVEL;
		static MsgType getStateMsgType(const QString& pState, PacePasswordId pPasswordId);

		[[nodiscard]] QByteArray toJson() const;
		[[nodiscard]] QByteArray getOutput() const;
		[[nodiscard]] bool isVoid() const;
		[[nodiscard]] MsgType getType() const;

		void setRequest(const QJsonObject& pRequest);

		operator Msg() const;
};

inline QDebug operator<<(QDebug pDbg, const MsgHandler& pMsg)
{
	QDebugStateSaver saver(pDbg);
	pDbg << pMsg.getType();
	return pDbg.space();
}


} // namespace governikus
