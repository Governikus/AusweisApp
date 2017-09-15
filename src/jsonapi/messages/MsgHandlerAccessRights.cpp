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

	const auto& jsonRaw = pObj["chat"];
	if (jsonRaw.isUndefined())
	{
		setError(QLatin1String("'chat' cannot be undefined"));
	}
	else if (!jsonRaw.isArray())
	{
		setError(QLatin1String("Invalid 'chat' data"));
	}
	else
	{
		handleSetChatData(jsonRaw.toArray(), ctx);
	}

	fillAccessRights(ctx);
}


void MsgHandlerAccessRights::handleSetChatData(const QJsonArray& pChat, const QSharedPointer<AuthContext>& pContext)
{
	Q_ASSERT(pContext);

	QSet<AccessRight> effectiveChat;

	if (!pContext->getOptionalAccessRights().isEmpty())
	{
		for (const auto& entry : pChat)
		{
			if (entry.isString())
			{
				const auto& func = [&](AccessRight pRight){
							if (pContext->getOptionalAccessRights().contains(pRight))
							{
								effectiveChat += pRight;
							}
							else
							{
								setError(QLatin1String("Entry in 'chat' data is not available"));
							}
						};

				if (!AccessRoleAndRightsUtil::fromTechnicalName(entry.toString().toLatin1().constData(), func))
				{
					setError(QLatin1String("Entry in 'chat' data is invalid"));
				}
			}
			else
			{
				setError(QLatin1String("Entry in 'chat' data needs to be string"));
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
		const QLatin1String name = AccessRoleAndRightsUtil::toTechnicalName(entry);
		if (name.size())
		{
			array += name;
		}
	}

	return array;
}


void MsgHandlerAccessRights::fillAccessRights(const QSharedPointer<const AuthContext>& pContext)
{
	Q_ASSERT(pContext);

	QJsonObject chat;
	chat["required"] = getAccessRights(pContext->getRequiredAccessRights());
	chat["optional"] = getAccessRights(pContext->getOptionalAccessRights());
	chat["effective"] = getAccessRights(pContext->getEffectiveAccessRights());

	mJsonObject["chat"] = chat;
	const auto& transactionInfo = pContext->getDidAuthenticateEac1()->getTransactionInfo();
	if (!transactionInfo.isEmpty())
	{
		mJsonObject["transactionInfo"] = transactionInfo;
	}

	const QJsonObject& aux = getAuxiliaryData(pContext);
	if (!aux.isEmpty())
	{
		mJsonObject["aux"] = aux;
	}
}


QJsonObject MsgHandlerAccessRights::getAuxiliaryData(const QSharedPointer<const AuthContext>& pContext)
{
	QJsonObject obj;

	const auto& eac1 = pContext->getDidAuthenticateEac1();
	if (eac1)
	{
		const auto& aux = eac1->getAuthenticatedAuxiliaryData();
		if (aux)
		{
			if (aux->hasAgeVerificationDate())
			{
				obj["ageVerificationDate"] = aux->getAgeVerificationDate().toString(Qt::ISODate);
				obj["requiredAge"] = aux->getRequiredAge();
			}

			if (aux->hasValidityDate())
			{
				obj["validityDate"] = aux->getValidityDate().toString(Qt::ISODate);
			}

			if (aux->hasCommunityID())
			{
				obj["communityId"] = QString::fromUtf8(aux->getCommunityID());
			}
		}
	}

	return obj;
}


void MsgHandlerAccessRights::setError(const QLatin1String& pError)
{
	mJsonObject["error"] = pError;
}
