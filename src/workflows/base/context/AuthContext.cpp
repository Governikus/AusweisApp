/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "AuthContext.h"

#include "AppSettings.h"
#include "SecureStorage.h"
#include "asn1/Chat.h"
#include "paos/retrieve/DidAuthenticateEac1Parser.h"


using namespace governikus;


AuthContext::AuthContext(const Action pAction, bool pActivateUi, const QUrl& pActivationUrl, const BrowserHandler& pHandler)
	: WorkflowContext(pAction, pActivateUi)
	, mTcTokenNotFound(true)
	, mErrorReportedToServer(false)
	, mReceivedBrowserSendFailed(false)
	, mSkipMobileRedirect(false)
	, mChangeTransportPin(false)
	, mActivationUrl(pActivationUrl)
	, mTcTokenUrl()
	, mTcToken()
	, mRefreshUrl()
	, mReceivedMessageId()
	, mStartPaos()
	, mInitializeFramework()
	, mInitializeFrameworkResponse()
	, mDIDAuthenticateEAC1()
	, mDIDAuthenticateResponseEAC1()
	, mDIDAuthenticateEAC2()
	, mDIDAuthenticateResponseEACAdditionalInputType()
	, mDIDAuthenticateEACAdditionalInputType()
	, mDIDAuthenticateResponseEAC2()
	, mTransmit()
	, mTransmitResponse()
	, mStartPaosResponse()
	, mAccessRightManager()
	, mCertificates()
	, mDvCvc()
	, mCvcChainBuilderProd()
	, mCvcChainBuilderTest()
	, mSslSession()
	, mBrowserHandler(pHandler)
{
}


AuthContext::AuthContext(bool pActivateUi, const QUrl& pActivationUrl, const BrowserHandler& pHandler)
	: AuthContext(Action::AUTH, pActivateUi, pActivationUrl, pHandler)
{
}


void AuthContext::setReceivedBrowserSendFailed(bool pReceivedBrowserSendFailed)
{
	if (mReceivedBrowserSendFailed != pReceivedBrowserSendFailed)
	{
		mReceivedBrowserSendFailed = pReceivedBrowserSendFailed;
		Q_EMIT fireResultChanged();
	}
}


void AuthContext::requestChangeTransportPin()
{
	if (mChangeTransportPin)
	{
		return;
	}

	setMobileSkipRedirect();
	mChangeTransportPin = true;
	Q_EMIT fireShowChangePinViewChanged();
}


void AuthContext::initAccessRightManager(const QSharedPointer<const CVCertificate>& pTerminalCvc)
{
	mAccessRightManager.reset(new AccessRightManager(mDIDAuthenticateEAC1, pTerminalCvc, mDvCvc));
	connect(mAccessRightManager.data(), &AccessRightManager::fireEffectiveAccessRightsChanged, this, &AuthContext::fireCanAllowedModeChanged);
	Q_EMIT fireAccessRightManagerCreated(mAccessRightManager);
	Q_EMIT fireCanAllowedModeChanged();
}


bool AuthContext::isCanAllowedMode() const
{
	return mAccessRightManager && mAccessRightManager->getEffectiveAccessRights().contains(AccessRight::CAN_ALLOWED);
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
	if (!mAccessRightManager)
	{
		return QByteArray();
	}

	return *mAccessRightManager;
}


CVCertificateChain AuthContext::getChainStartingWith(const QSharedPointer<const CVCertificate>& pChainRoot) const
{
	const auto& productionChain = mCvcChainBuilderProd.getChainStartingWith(pChainRoot);
	if (productionChain.isValid())
	{
		qDebug() << "Found chain within productive PKI.";
		return productionChain;
	}

	const auto& testChain = mCvcChainBuilderTest.getChainStartingWith(pChainRoot);
	if (testChain.isValid())
	{
		qDebug() << "Found chain within test PKI.";
		return testChain;
	}

	return CVCertificateChain();
}


bool AuthContext::hasChainForCertificationAuthority(const EstablishPaceChannelOutput& pPaceOutput) const
{
	return getChainForCertificationAuthority(pPaceOutput).isValid();
}


CVCertificateChain AuthContext::getChainForCertificationAuthority(const EstablishPaceChannelOutput& pPaceOutput) const
{
	const auto& productionChain = mCvcChainBuilderProd.getChainForCertificationAuthority(pPaceOutput);
	if (productionChain.isValid())
	{
		return productionChain;
	}

	return mCvcChainBuilderTest.getChainForCertificationAuthority(pPaceOutput);
}


void AuthContext::initCvcChainBuilder(const QList<QSharedPointer<const CVCertificate>>& pAdditionalCertificates)
{
	Q_ASSERT(mDIDAuthenticateEAC1);

	QList<QSharedPointer<const CVCertificate>> cvcs;
	cvcs += CVCertificate::fromRaw(Env::getSingleton<AppSettings>()->getPreVerificationSettings().getLinkCertificates());
	cvcs += getDidAuthenticateEac1()->getCvCertificates();
	cvcs += pAdditionalCertificates;

	const auto* secureStorage = Env::getSingleton<SecureStorage>();
	mCvcChainBuilderProd = CVCertificateChainBuilder(cvcs + CVCertificate::fromRaw(secureStorage->getCVRootCertificates(true)), true);
	mCvcChainBuilderTest = CVCertificateChainBuilder(cvcs + CVCertificate::fromRaw(secureStorage->getCVRootCertificates(false)), false);
}
