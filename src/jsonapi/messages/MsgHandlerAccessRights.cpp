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

	QSet<AccessRight> effectiveChat;

	if (!pContext->getOptionalAccessRights().isEmpty())
	{
		for (const auto& entry : pRaw)
		{
			if (entry.isDouble())
			{
				const auto value = static_cast<AccessRight>(entry.toInt());
				if (pContext->getOptionalAccessRights().contains(value))
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


QJsonArray MsgHandlerAccessRights::getAccessRights(const QSet<AccessRight>& pRights) const
{
	QJsonArray array;

	QList<AccessRight> accessRights = pRights.toList();
	std::sort(accessRights.rbegin(), accessRights.rend());
	for (auto entry : qAsConst(accessRights))
	{
		array += static_cast<int>(entry);
	}

	return array;
}


void MsgHandlerAccessRights::fillAccessRights(const QSharedPointer<const AuthContext>& pContext)
{
	Q_ASSERT(pContext);

	QJsonObject raw;
	raw["required"] = getAccessRights(pContext->getRequiredAccessRights());
	raw["optional"] = getAccessRights(pContext->getOptionalAccessRights());
	raw["effective"] = getAccessRights(pContext->getEffectiveAccessRights());

	mJsonObject["raw"] = raw;
}


void MsgHandlerAccessRights::setError(const QLatin1String& pError)
{
	mJsonObject["error"] = pError;
}
