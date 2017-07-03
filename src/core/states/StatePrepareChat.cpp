/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "states/StatePrepareChat.h"

using namespace governikus;


/**
 * Performs sanity operations on a chat:
 * <ul>
 *  <li>all access rights not contained in the corresponding CVC are removed</li>
 *  <li>all access rights not displayed are removed</li>
 * </ul>
 */
static QSharedPointer<CHAT> sanitizeChat(QSharedPointer<CHAT> pChatToSanitize, QSharedPointer<AuthContext> pAuthContext, bool pIsRequiredChat)
{
	auto terminalCvc = pAuthContext->getTerminalCvc();

	const auto notAllowedAccessRight = pChatToSanitize->getAccessRights() - terminalCvc->getBody().getCHAT().getAccessRights();
	const auto notDisplayedAccessRight = pChatToSanitize->getAccessRights() - AccessRoleAndRightsUtil::allDisplayedOrderedRights().toSet();

	if (!notAllowedAccessRight.isEmpty())
	{
		for (AccessRight accessRight : notAllowedAccessRight)
		{
			qWarning() << "Access right" << accessRight << "in" << (pIsRequiredChat ? "required" : "optional") << "chat but not in CVC!";
			pChatToSanitize->removeAccessRight(accessRight);
		}
	}
	if (!notDisplayedAccessRight.isEmpty())
	{
		for (AccessRight accessRight : notDisplayedAccessRight)
		{
			qWarning() << "Access right" << accessRight << "in" << (pIsRequiredChat ? "required" : "optional") << "chat but not displayed!";
			pChatToSanitize->removeAccessRight(accessRight);
		}
	}
	QSharedPointer<AuthenticatedAuxiliaryData> auxiliaryData = pAuthContext->getDidAuthenticateEac1()->getAuthenticatedAuxiliaryData();
	if (pChatToSanitize->hasAccessRight(AccessRight::AGE_VERIFICATION) && (!auxiliaryData || !auxiliaryData->hasAgeVerificationDate()))
	{
		qWarning() << "AGE_VERIFICATION requested, but no age specified";
		pChatToSanitize->removeAccessRight(AccessRight::AGE_VERIFICATION);
	}
	if (pChatToSanitize->hasAccessRight(AccessRight::COMMUNITY_ID_VERIFICATION) && (!auxiliaryData || !auxiliaryData->hasCommunityID()))
	{
		qWarning() << "COMMUNITY_ID_VERIFICATION requested, but no community id specified";
		pChatToSanitize->removeAccessRight(AccessRight::COMMUNITY_ID_VERIFICATION);
	}

	return pChatToSanitize;
}


StatePrepareChat::StatePrepareChat(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StatePrepareChat::run()
{
	Q_ASSERT(getContext()->getDidAuthenticateEac1());
	Q_ASSERT(getContext()->getTerminalCvc());
	if (auto auxData = getContext()->getDidAuthenticateEac1()->getAuthenticatedAuxiliaryData())
	{
		getContext()->setRequiredAge(auxData->getRequiredAge());
	}
	if (getContext()->getDidAuthenticateEac1()->getRequiredChat())
	{
		QSharedPointer<CHAT> requiredChat(new CHAT(*getContext()->getDidAuthenticateEac1()->getRequiredChat().data()));
		getContext()->setRequiredChat(sanitizeChat(requiredChat, getContext(), true));
	}

	const CHAT& terminalChat = getContext()->getTerminalCvc()->getBody().getCHAT();

	if (getContext()->getDidAuthenticateEac1()->getOptionalChat())
	{
		QSharedPointer<CHAT> optionalChat(new CHAT(*getContext()->getDidAuthenticateEac1()->getOptionalChat().data()));

		if (getContext()->getRequiredChat())
		{
			const auto accessRights = getContext()->getRequiredChat()->getAccessRights();
			for (AccessRight requiredRight : accessRights)
			{
				optionalChat->removeAccessRight(requiredRight);
			}
		}
		getContext()->setOptionalChat(sanitizeChat(optionalChat, getContext(), false));
	}
	else if (!getContext()->getDidAuthenticateEac1()->getOptionalChat() && !getContext()->getDidAuthenticateEac1()->getRequiredChat())
	{
		QSharedPointer<CHAT> optionalChat(new CHAT(terminalChat));
		getContext()->setOptionalChat(sanitizeChat(optionalChat, getContext(), false));
	}

	QSharedPointer<CHAT> effectiveChat(new CHAT(terminalChat));
	effectiveChat->removeAllAccessRights();
	if (getContext()->getRequiredChat())
	{
		effectiveChat->setAccessRights(getContext()->getRequiredChat()->getAccessRights());
	}
	if (getContext()->getOptionalChat())
	{
		effectiveChat->setAccessRights(getContext()->getOptionalChat()->getAccessRights());
	}
	getContext()->setEffectiveChat(effectiveChat);

	qDebug() << "Initially set effective chat to:" << getContext()->getEffectiveChat()->encode().toHex();
	Q_EMIT fireSuccess();
}
