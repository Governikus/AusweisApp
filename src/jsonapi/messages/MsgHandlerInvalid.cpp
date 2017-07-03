/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MsgHandlerInvalid.h"

using namespace governikus;

MsgHandlerInvalid::MsgHandlerInvalid(const QString& pError)
	: MsgHandler(MsgType::INVALID, "error", pError)
{
}


MsgHandlerInvalid::MsgHandlerInvalid(const QLatin1String& pError)
	: MsgHandler(MsgType::INVALID, "error", pError)
{
}


MsgHandlerInvalid::MsgHandlerInvalid(const QJsonParseError& pError)
	: MsgHandlerInvalid(QStringLiteral("%1 (offset: %2)").arg(pError.errorString(), QString::number(pError.offset)))
{
}
