/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MsgHandlerAccessRights.h"

#include <QJsonArray>

using namespace governikus;

MsgHandlerAccessRights::MsgHandlerAccessRights(const MsgContext& pContext)
	: MsgHandler(MsgType::ACCESS_RIGHTS)
{
	Q_ASSERT(pContext.getAuthContext());
	fillAccessRights(pContext.getAuthContext());
}


MsgHandlerAccessRights::MsgHandlerAccessRights(const QJsonObject& pObj, MsgContext& pContext)
	: MsgHandler(MsgType::ACCESS_RIGHTS)
{
	auto ctx = pContext.getAuthContext();
	Q_ASSERT(ctx);

	const auto& jsonRaw = pObj["raw"];
	if (jsonRaw.isUndefined())
	{
		setError(QLatin1String("'raw' cannot be undefined"));
	}
	else if (!jsonRaw.isArray())
	{
		setError(QLatin1String("Invalid 'raw' data"));
	}
	else
	{
		handleSetRawData(jsonRaw.toArray(), ctx);
	}

	fillAccessRights(ctx);
}


void MsgHandlerAccessRights::handleSetRawData(const QJsonArray& pRaw, const QSharedPointer<AuthContext>& pContext)
{
	Q_ASSERT(pContext);
	Q_ASSERT(pContext->getEffectiveChat());

	QSet<AccessRight> effectiveChat;

	if (pContext->getOptionalChat())
	{
		for (const auto& entry : pRaw)
		{
			if (entry.isDouble())
			{
				const auto value = static_cast<AccessRight>(entry.toInt());
				if (pContext->getOptionalChat()->hasAccessRight(value))
				{
					effectiveChat += value;
				}
				else
				{
					setError(QLatin1String("Entry in 'raw' data is invalid"));
				}
			}
			else
			{
				setError(QLatin1String("Entry in 'raw' data needs to be integer"));
			}
		}
	}
	else
	{
		setError(QLatin1String("No optional access rights available"));
	}


	if (!mJsonObject.contains("error"))
	{
		pContext->setEffectiveAccessRights(effectiveChat);
	}
}


QJsonArray MsgHandlerAccessRights::getAccessRights(const QSharedPointer<const CHAT>& pChat) const
{
	QJsonArray array;
	if (pChat)
	{
		QList<AccessRight> accessRights = pChat->getAccessRights().toList();
		std::sort(accessRights.rbegin(), accessRights.rend());
		for (auto entry : qAsConst(accessRights))
		{
			array += static_cast<int>(entry);
		}
	}
	return array;
}


void MsgHandlerAccessRights::fillAccessRights(const QSharedPointer<const AuthContext>& pContext)
{
	Q_ASSERT(pContext);

	QJsonObject raw;
	raw["required"] = getAccessRights(pContext->getRequiredChat());
	raw["optional"] = getAccessRights(pContext->getOptionalChat());
	raw["effective"] = getAccessRights(pContext->getEffectiveChat());

	mJsonObject["raw"] = raw;
}


void MsgHandlerAccessRights::setError(const QLatin1String& pError)
{
	mJsonObject["error"] = pError;
}
