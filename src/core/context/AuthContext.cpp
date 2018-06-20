/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "AuthContext.h"

#include "asn1/Chat.h"
#include "AppSettings.h"
#include "Env.h"
#include "paos/retrieve/DidAuthenticateEac1Parser.h"
#include "SecureStorage.h"

#include <QSignalBlocker>

using namespace governikus;

AuthContext::AuthContext(const QSharedPointer<ActivationContext>& pActivationContext)
	: WorkflowContext()
	, mTcTokenNotFound(true)
	, mErrorReportedToServer(false)
	, mActivationContext(pActivationContext)
	, mTcTokenUrl()
	, mTcToken()
	, mRefreshUrl()
	, mMessageIdHandler(new MessageIdHandler())
	, mStartPaos()
	, mInitializeFramework()
	, mInitializeFrameworkResponse()
	, mDIDList()
	, mDIDListResponse()
	, mDIDAuthenticateEAC1()
	, mDIDAuthenticateResponseEAC1()
	, mDIDAuthenticateEAC2()
	, mDIDAuthenticateResponseEACAdditionalInputType()
	, mDIDAuthenticateEACAdditionalInputType()
	, mDIDAuthenticateResponseEAC2()
	, mTransmits()
	, mTransmitResponses()
	, mTransmitResponseFailed(false)
	, mDisconnectResponse()
	, mStartPaosResponse()
	, mEffectiveAccessRights()
	, mRequiredAccessRights()
	, mOptionalAccessRights()
	, mCertificates()
	, mTerminalCvc()
	, mDvCvc()
	, mCvcChainBuilderProd()
	, mCvcChainBuilderTest()
	, mSslSession()
{
}


AuthContext::~AuthContext()
{
}


void AuthContext::initializeChat()
{
	Q_ASSERT(mTerminalCvc);
	Q_ASSERT(mDIDAuthenticateEAC1);

	mRequiredAccessRights.clear();
	if (const auto& requiredChat = mDIDAuthenticateEAC1->getRequiredChat())
	{
		mRequiredAccessRights = requiredChat.data()->getAccessRights();
		removeForbiddenAccessRights(mRequiredAccessRights);
	}

	mOptionalAccessRights.clear();
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

	mEffectiveAccessRights = mRequiredAccessRights + mOptionalAccessRights;

	Q_EMIT fireAuthenticationDataChanged();
	Q_EMIT fireEffectiveChatChanged();
}


bool AuthContext::removeForbiddenAccessRights(QSet<AccessRight>& pAccessRights)
{
	bool changed = false;

	if (!mTerminalCvc)
	{
		return changed;
	}

	const auto& allowedCvcAccessRights = mTerminalCvc->getBody().getCHAT().getAccessRights();
	const auto& allDisplayedOrderedRights = AccessRoleAndRightsUtil::allDisplayedOrderedRights().toSet();
	const auto& allowedAccessRights = allowedCvcAccessRights & allDisplayedOrderedRights;

	const auto rightsToCheck = pAccessRights;
	for (const AccessRight& accessRight : rightsToCheck)
	{
		if (allowedAccessRights.contains(accessRight))
		{
			continue;
		}
		qWarning() << "Access right" << accessRight << "is used in chat but it's not included in CVC or it's not allowed to display!";
		pAccessRights.remove(accessRight);
		changed = true;
	}

	if (!mDIDAuthenticateEAC1)
	{
		return changed;
	}

	const auto& auxiliaryData = mDIDAuthenticateEAC1->getAuthenticatedAuxiliaryData();
	if (pAccessRights.contains(AccessRight::AGE_VERIFICATION))
	{
		if (!auxiliaryData || !auxiliaryData->hasAgeVerificationDate())
		{
			qWarning() << "AGE_VERIFICATION requested, but no age specified";
			pAccessRights.remove(AccessRight::AGE_VERIFICATION);
			changed = true;
		}
	}
	if (pAccessRights.contains(AccessRight::COMMUNITY_ID_VERIFICATION))
	{
		if (!auxiliaryData || !auxiliaryData->hasCommunityID())
		{
			qWarning() << "COMMUNITY_ID_VERIFICATION requested, but no community id specified";
			pAccessRights.remove(AccessRight::COMMUNITY_ID_VERIFICATION);
			changed = true;
		}
	}

	return changed;
}


bool AuthContext::addEffectiveAccessRight(AccessRight pAccessRight)
{
	if (mEffectiveAccessRights.contains(pAccessRight))
	{
		return true;
	}

	if (mOptionalAccessRights.contains(pAccessRight))
	{
		mEffectiveAccessRights += pAccessRight;
		Q_EMIT fireEffectiveChatChanged();
		return true;
	}

	return false;
}


bool AuthContext::removeEffectiveAccessRight(AccessRight pAccessRight)
{
	if (mRequiredAccessRights.contains(pAccessRight))
	{
		return false;
	}

	if (mEffectiveAccessRights.contains(pAccessRight))
	{
		mEffectiveAccessRights -= pAccessRight;
		Q_EMIT fireEffectiveChatChanged();
	}

	return true;
}


bool AuthContext::setEffectiveAccessRights(const QSet<AccessRight>& pAccessRights)
{
	const auto oldRights = mEffectiveAccessRights;

	mEffectiveAccessRights.clear();
	if (!mRequiredAccessRights.isEmpty())
	{
		mEffectiveAccessRights += mRequiredAccessRights;
	}
	for (const auto& effectiveRight : pAccessRights)
	{
		const QSignalBlocker blocker(this);
		addEffectiveAccessRight(effectiveRight);
	}

	if (oldRights != mEffectiveAccessRights)
	{
		Q_EMIT fireEffectiveChatChanged();
	}

	return pAccessRights == mEffectiveAccessRights;
}


void AuthContext::setTerminalCvc(const QSharedPointer<const CVCertificate>& pTerminalCvc)
{
	mTerminalCvc = pTerminalCvc;

	const CHAT& terminalChat = mTerminalCvc->getBody().getCHAT();
	bool canAllowed = terminalChat.getAccessRights().contains(AccessRight::CAN_ALLOWED);
	setCanAllowedMode(canAllowed);
	qDebug() << "CAN allowed mode:" << canAllowed;

	initializeChat();
}


const QByteArray& AuthContext::getSslSession() const
{
	return mSslSession;
}


void AuthContext::setSslSession(const QByteArray& pSession)
{
	mSslSession = pSession;
}


QByteArray AuthContext::encodeEffectiveChat()
{
	Q_ASSERT(mTerminalCvc);

	CHAT effectiveChat(mTerminalCvc->getBody().getCHAT());
	effectiveChat.removeAllAccessRights();
	effectiveChat.setAccessRights(mEffectiveAccessRights);

	if (isCanAllowedMode())
	{
		qDebug() << "Enabling \"CAN allowed\" in effective CHAT.";
		effectiveChat.setAccessRight(AccessRight::CAN_ALLOWED);
	}

	qDebug() << "Using effective CHAT:" << effectiveChat.encode().toHex();
	return effectiveChat.encode();
}


CVCertificateChain AuthContext::getChainStartingWith(const QSharedPointer<const CVCertificate>& pChainRoot) const
{
	const auto& productionChain = mCvcChainBuilderProd.getChainStartingWith(pChainRoot);
	if (productionChain.isValid())
	{
		return productionChain;
	}

	return mCvcChainBuilderTest.getChainStartingWith(pChainRoot);
}


bool AuthContext::hasChainForCertificationAuthority(const EstablishPACEChannelOutput& pPaceOutput) const
{
	return getChainForCertificationAuthority(pPaceOutput).isValid();
}


CVCertificateChain AuthContext::getChainForCertificationAuthority(const EstablishPACEChannelOutput& pPaceOutput) const
{
	const auto& productionChain = mCvcChainBuilderProd.getChainForCertificationAuthority(pPaceOutput);
	if (productionChain.isValid())
	{
		return productionChain;
	}

	return mCvcChainBuilderTest.getChainForCertificationAuthority(pPaceOutput);
}


void AuthContext::initCvcChainBuilder(const QVector<QSharedPointer<const CVCertificate> >& pAdditionalCertificates)
{
	Q_ASSERT(mDIDAuthenticateEAC1);

	QVector<QSharedPointer<const CVCertificate> > cvcs;
	cvcs += CVCertificate::fromHex(Env::getSingleton<AppSettings>()->getPreVerificationSettings().getLinkCertificates());
	cvcs += getDidAuthenticateEac1()->getCvCertificates();
	cvcs += pAdditionalCertificates;

	const SecureStorage& secureStorage = SecureStorage::getInstance();
	mCvcChainBuilderProd = CVCertificateChainBuilder(cvcs + CVCertificate::fromHex(secureStorage.getCVRootCertificates(true)), true);
	mCvcChainBuilderTest = CVCertificateChainBuilder(cvcs + CVCertificate::fromHex(secureStorage.getCVRootCertificates(false)), false);
}


QString AuthContext::getRequiredAge() const
{
	Q_ASSERT(mDIDAuthenticateEAC1);
	Q_ASSERT(mDIDAuthenticateEAC1->getAuthenticatedAuxiliaryData());

	if (mDIDAuthenticateEAC1 && mDIDAuthenticateEAC1->getAuthenticatedAuxiliaryData())
	{
		return mDIDAuthenticateEAC1->getAuthenticatedAuxiliaryData()->getRequiredAge();
	}
	return QString();
}
