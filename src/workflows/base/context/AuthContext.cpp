/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#include "AuthContext.h"

#include "AppSettings.h"
#include "SecureStorage.h"


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


const QList<QSharedPointer<const CVCertificate>>& AuthContext::logCertificates(const QString& pSource, const QList<QSharedPointer<const CVCertificate>>& pCertificates) const
{
	if (!pCertificates.isEmpty())
	{
		qCDebug(card) << "    Source:" << pSource;
		for (const auto& cvc : pCertificates)
		{
			qCDebug(card) << "        " << cvc;
		}
	}
	return pCertificates;
}


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
	, mCvcChainBuilder()
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


const QByteArray& AuthContext::getSslSessionPsk() const
{
	return mSslSessionPsk;
}


void AuthContext::setSslSessionPsk(const QByteArray& pSession)
{
	mSslSessionPsk = pSession;
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
	const auto& chain = mCvcChainBuilder.getChainStartingWith(pChainRoot);
	if (chain.isValid())
	{
		return chain;
	}

	return CVCertificateChain();
}


bool AuthContext::hasChainForCertificationAuthority(const EstablishPaceChannelOutput& pPaceOutput) const
{
	return getChainForCertificationAuthority(pPaceOutput).isValid();
}


CVCertificateChain AuthContext::getChainForCertificationAuthority(const EstablishPaceChannelOutput& pPaceOutput) const
{
	return mCvcChainBuilder.getChainForCertificationAuthority(pPaceOutput);
}


void AuthContext::initCvcChainBuilder(const QList<QSharedPointer<const CVCertificate>>& pAdditionalCertificates)
{
	Q_ASSERT(mDIDAuthenticateEAC1);

	qCDebug(card) << "Initialize ChainBuilder with certificates";

	QList<QSharedPointer<const CVCertificate>> cvcs;
	cvcs << logCertificates("PreVerification"_L1, CVCertificate::fromRaw(Env::getSingleton<AppSettings>()->getPreVerificationSettings().getLinkCertificates()));
	cvcs << logCertificates("Eac1"_L1, getDidAuthenticateEac1()->getCvCertificates());
	cvcs << logCertificates("Eac2"_L1, pAdditionalCertificates);
	const auto* secureStorage = Env::getSingleton<SecureStorage>();
	cvcs << logCertificates("Productive"_L1, CVCertificate::fromRaw(secureStorage->getCVRootCertificates(true)));
	cvcs << logCertificates("Test"_L1, CVCertificate::fromRaw(secureStorage->getCVRootCertificates(false)));

	mCvcChainBuilder = CVCertificateChainBuilder(cvcs);
}
