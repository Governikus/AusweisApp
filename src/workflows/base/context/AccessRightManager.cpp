/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "AccessRightManager.h"

#include "AppSettings.h"
#include "VolatileSettings.h"

using namespace governikus;

AccessRightManager::AccessRightManager(QSharedPointer<DIDAuthenticateEAC1> pDIDAuthenticateEAC1, QSharedPointer<const CVCertificate> pTerminalCvc)
	: QObject()
	, mTerminalCvc(pTerminalCvc)
	, mDIDAuthenticateEAC1(pDIDAuthenticateEAC1)
	, mOptionalAccessRights()
	, mEffectiveAccessRights()
	, mRequiredAccessRights()
{
	if (!mTerminalCvc || !mDIDAuthenticateEAC1)
	{
		return;
	}

	if (const auto& requiredChat = mDIDAuthenticateEAC1->getRequiredChat())
	{
		mRequiredAccessRights = requiredChat.data()->getAccessRights();
		removeForbiddenAccessRights(mRequiredAccessRights);
	}

	if (const auto& optionalChat = mDIDAuthenticateEAC1->getOptionalChat())
	{
		mOptionalAccessRights = optionalChat.data()->getAccessRights();
		removeForbiddenAccessRights(mOptionalAccessRights);

		if (mOptionalAccessRights.size() > 0 && mRequiredAccessRights.size() > 0)
		{
			mOptionalAccessRights -= mRequiredAccessRights;
		}
	}

	if (mOptionalAccessRights.isEmpty() && mRequiredAccessRights.isEmpty())
	{
		mOptionalAccessRights = mTerminalCvc->getBody().getCHAT().getAccessRights();
		removeForbiddenAccessRights(mOptionalAccessRights);
	}

	mRequiredAccessRights -= AccessRight::CAN_ALLOWED;
	const bool canAllowed = mTerminalCvc->getBody().getCHAT().getAccessRights().contains(AccessRight::CAN_ALLOWED);
	const bool pinManagement = mTerminalCvc->getBody().getCHAT().getAccessRights().contains(AccessRight::PIN_MANAGEMENT);
	const bool canAllowedEnabled = Env::getSingleton<VolatileSettings>()->isUsedAsSDK() || Env::getSingleton<AppSettings>()->getGeneralSettings().isEnableCanAllowed();
	if (canAllowed && (canAllowedEnabled || pinManagement))
	{
		mOptionalAccessRights += AccessRight::CAN_ALLOWED;
	}
	else
	{
		mOptionalAccessRights -= AccessRight::CAN_ALLOWED;
	}

	mEffectiveAccessRights = mRequiredAccessRights + mOptionalAccessRights;
}


void AccessRightManager::removeForbiddenAccessRights(QSet<AccessRight>& pAccessRights)
{
	if (!mTerminalCvc)
	{
		return;
	}

	const auto& allowedCvcAccessRights = mTerminalCvc->getBody().getCHAT().getAccessRights();

	const auto rights = AccessRoleAndRightsUtil::allDisplayedOrderedRights();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
	const auto& allDisplayedOrderedRights = QSet<AccessRight>(rights.constBegin(), rights.constEnd());
#else
	const auto& allDisplayedOrderedRights = rights.toSet();
#endif

	const auto& allowedAccessRights = allowedCvcAccessRights & allDisplayedOrderedRights;

	const auto rightsToCheck = pAccessRights;
	for (const AccessRight& accessRight : rightsToCheck)
	{
		if (allowedAccessRights.contains(accessRight))
		{
			continue;
		}
		qWarning() << "Access right" << accessRight << "is used in chat but it's not included in CVC or it's not allowed to display!";
		pAccessRights -= accessRight;
	}

	if (!mDIDAuthenticateEAC1)
	{
		return;
	}

	const auto& auxiliaryData = mDIDAuthenticateEAC1->getAuthenticatedAuxiliaryData();
	if (pAccessRights.contains(AccessRight::AGE_VERIFICATION) && (!auxiliaryData || !auxiliaryData->hasAgeVerificationDate()))
	{
		qWarning() << "AGE_VERIFICATION requested, but no age specified";
		pAccessRights -= AccessRight::AGE_VERIFICATION;
	}
	if (pAccessRights.contains(AccessRight::COMMUNITY_ID_VERIFICATION) && (!auxiliaryData || !auxiliaryData->hasCommunityID()))
	{
		qWarning() << "COMMUNITY_ID_VERIFICATION requested, but no community id specified";
		pAccessRights -= AccessRight::COMMUNITY_ID_VERIFICATION;
	}
}


void AccessRightManager::operator+=(AccessRight pAccessRight)
{
	if (!mOptionalAccessRights.contains(pAccessRight) && !mRequiredAccessRights.contains(pAccessRight))
	{
		qWarning() << "Adding" << pAccessRight << "to effective chat is not allowed";
		return;
	}

	if (!mEffectiveAccessRights.contains(pAccessRight))
	{
		mEffectiveAccessRights += pAccessRight;
		qDebug() << "Added" << pAccessRight << "to effective chat";
		Q_EMIT fireEffectiveAccessRightsChanged();
	}
}


void AccessRightManager::operator-=(AccessRight pAccessRight)
{
	if (mRequiredAccessRights.contains(pAccessRight))
	{
		qWarning() << "Removing" << pAccessRight << "from effective chat is not allowed";
		return;
	}

	if (mEffectiveAccessRights.contains(pAccessRight))
	{
		mEffectiveAccessRights -= pAccessRight;
		qDebug() << "Removed" << pAccessRight << "from effective chat";
		Q_EMIT fireEffectiveAccessRightsChanged();
	}
}


void AccessRightManager::operator=(const QSet<AccessRight>& pAccessRights)
{
	mEffectiveAccessRights = mRequiredAccessRights;
	for (const auto& effectiveRight : pAccessRights)
	{
		const QSignalBlocker blocker(this);
		*this += effectiveRight;
	}
	Q_EMIT fireEffectiveAccessRightsChanged();
}


AccessRightManager::operator QByteArray() const
{
	Q_ASSERT(mTerminalCvc);

	CHAT effectiveChat(mTerminalCvc->getBody().getCHAT());
	effectiveChat.removeAllAccessRights();
	effectiveChat.setAccessRights(mEffectiveAccessRights);

	qDebug() << "Using effective CHAT:" << effectiveChat.encode().toHex() << "with access rights:" << effectiveChat.getAccessRights();

	return effectiveChat.encode();
}
