/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "AuthContext.h"

#include "AppSettings.h"
#include "paos/retrieve/DidAuthenticateEac1Parser.h"

#include <QSignalBlocker>

using namespace governikus;

AuthContext::AuthContext(ActivationContext* pActivationContext)
	: WorkflowContext()
	, mTcTokenNotFound(true)
	, mErrorReportedToServer(false)
	, mActivationContext(pActivationContext)
	, mNetworkManager(new NetworkManager())
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
	, mDisconnectResponse()
	, mStartPaosResponse()
	, mEffectiveChat()
	, mCertificates()
	, mTerminalCvc()
	, mDvCvc()
	, mCvcChainBuilderProd()
	, mCvcChainBuilderTest()
{
}


AuthContext::~AuthContext()
{
}


bool AuthContext::sanitizeEffectiveAccessRights()
{
	bool changed = false;

	if (!mEffectiveChat)
	{
		return changed;
	}

	const auto& effectiveAccessRights = mEffectiveChat->getAccessRights();
	for (const auto& right : effectiveAccessRights)
	{
		if (mRequiredChat && mRequiredChat->hasAccessRight(right))
		{
			continue;
		}
		if (mOptionalChat && mOptionalChat->hasAccessRight(right))
		{
			continue;
		}
		changed = true;
		mEffectiveChat->removeAccessRight(right);
		qWarning() << "Access right" << right << "was in effective chat but not in required or optional chat!";
	}

	if (!mRequiredChat)
	{
		return changed;
	}

	const auto& requiredAccessRights = mRequiredChat->getAccessRights();
	for (const auto& right : requiredAccessRights)
	{
		if (mEffectiveChat->hasAccessRight(right))
		{
			continue;
		}
		changed = true;
		mEffectiveChat->setAccessRight(right);
		qWarning() << "Access right" << right << "is required but was not in effective chat!";
	}

	return changed;
}


bool AuthContext::setEffectiveChat(const QSharedPointer<CHAT>& pEffectiveChat)
{
	mEffectiveChat = pEffectiveChat;
	bool changed = sanitizeEffectiveAccessRights();
	Q_EMIT fireEffectiveChatChanged();
	return !changed;
}


bool AuthContext::addEffectiveAccessRight(AccessRight pAccessRight)
{
	Q_ASSERT(mEffectiveChat);

	if (mEffectiveChat->hasAccessRight(pAccessRight))
	{
		return true;
	}

	if (mRequiredChat && mRequiredChat->hasAccessRight(pAccessRight))
	{
		mEffectiveChat->setAccessRight(pAccessRight);
		Q_EMIT fireEffectiveChatChanged();
		return true;
	}
	if (mOptionalChat && mOptionalChat->hasAccessRight(pAccessRight))
	{
		mEffectiveChat->setAccessRight(pAccessRight);
		Q_EMIT fireEffectiveChatChanged();
		return true;
	}

	return false;
}


bool AuthContext::removeEffectiveAccessRight(AccessRight pAccessRight)
{
	Q_ASSERT(mEffectiveChat);

	if (mRequiredChat && mRequiredChat->hasAccessRight(pAccessRight))
	{
		return false;
	}

	if (mEffectiveChat->hasAccessRight(pAccessRight))
	{
		mEffectiveChat->removeAccessRight(pAccessRight);
		Q_EMIT fireEffectiveChatChanged();
	}

	return true;
}


bool AuthContext::setEffectiveAccessRights(const QSet<AccessRight>& pAccessRights)
{
	Q_ASSERT(mEffectiveChat);

	const auto oldRights = mEffectiveChat->getAccessRights();

	mEffectiveChat->removeAllAccessRights();
	for (auto& right : pAccessRights)
	{
		const QSignalBlocker blocker(this);
		addEffectiveAccessRight(right);
	}
	sanitizeEffectiveAccessRights();

	if (oldRights != mEffectiveChat->getAccessRights())
	{
		Q_EMIT fireEffectiveChatChanged();
	}

	return pAccessRights == mEffectiveChat->getAccessRights();
}


void AuthContext::setOptionalChat(const QSharedPointer<CHAT>& pOptionalChat)
{
	mOptionalChat = pOptionalChat;
	if (sanitizeEffectiveAccessRights())
	{
		Q_EMIT fireEffectiveChatChanged();
	}
	Q_EMIT fireOptionalChatChanged();
}


void AuthContext::setRequiredChat(const QSharedPointer<CHAT>& pRequiredChat)
{
	mRequiredChat = pRequiredChat;
	if (sanitizeEffectiveAccessRights())
	{
		Q_EMIT fireEffectiveChatChanged();
	}
	Q_EMIT fireRequiredChatChanged();
}


CVCertificateChain AuthContext::getChainStartingWith(const QSharedPointer<CVCertificate>& pChainRoot) const
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


void AuthContext::initCvcChainBuilder(const QVector<QSharedPointer<CVCertificate> >& pAdditionalCertificates)
{
	Q_ASSERT(mDIDAuthenticateEAC1);

	QVector<QSharedPointer<CVCertificate> > cvcs;
	cvcs += CVCertificate::fromHex(AppSettings::getInstance().getPreVerificationSettings().getLinkCertificates());
	cvcs += getDidAuthenticateEac1()->getCvCertificates();
	cvcs += pAdditionalCertificates;

	const auto& secureStorage = AppSettings::getInstance().getSecureStorage();
	mCvcChainBuilderProd = CVCertificateChainBuilder(cvcs + CVCertificate::fromHex(secureStorage.getCVRootCertificates(true)), true);
	mCvcChainBuilderTest = CVCertificateChainBuilder(cvcs + CVCertificate::fromHex(secureStorage.getCVRootCertificates(false)), false);
}
