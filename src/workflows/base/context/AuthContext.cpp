/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "AuthContext.h"

#include "asn1/Chat.h"
#include "AppSettings.h"
#include "GeneralSettings.h"
#include "paos/retrieve/DidAuthenticateEac1Parser.h"
#include "SecureStorage.h"

#include <QSignalBlocker>

using namespace governikus;

AuthContext::AuthContext(const QSharedPointer<ActivationContext>& pActivationContext)
	: WorkflowContext()
	, mProgressValue(0)
	, mProgressMessage()
	, mTcTokenNotFound(true)
	, mErrorReportedToServer(false)
	, mSkipRedirect(false)
	, mActivationContext(pActivationContext)
	, mTcTokenUrl()
	, mTcToken()
	, mRefreshUrl()
	, mReceivedMessageId()
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
	, mAccessRightManager()
	, mCertificates()
	, mDvCvc()
	, mCvcChainBuilderProd()
	, mCvcChainBuilderTest()
	, mSslSession()
{
	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	connect(&generalSettings, &GeneralSettings::fireLanguageChanged, this, &AuthContext::fireProgressChanged);
}


void AuthContext::setProgress(int pValue, const QString& pMessage)
{
	if (pValue != mProgressValue || pMessage != mProgressMessage)
	{
		mProgressValue = pValue;
		mProgressMessage = pMessage;

		const auto& connection = getCardConnection();
		if (connection)
		{
			// Card interaction makes up about 80 % of the entire workflow's duration,
			// "correct" the relative progress value accordingly.
			connection->setProgressMessage(pMessage, static_cast<int>(1.25 * pValue));
		}

		Q_EMIT fireProgressChanged();
	}
}


void AuthContext::initAccessRightManager(const QSharedPointer<const CVCertificate>& pTerminalCvc)
{
	mAccessRightManager.reset(new AccessRightManager(mDIDAuthenticateEAC1, pTerminalCvc));
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


void AuthContext::initCvcChainBuilder(const QVector<QSharedPointer<const CVCertificate> >& pAdditionalCertificates)
{
	Q_ASSERT(mDIDAuthenticateEAC1);

	QVector<QSharedPointer<const CVCertificate> > cvcs;
	cvcs += CVCertificate::fromHex(Env::getSingleton<AppSettings>()->getPreVerificationSettings().getLinkCertificates());
	cvcs += getDidAuthenticateEac1()->getCvCertificates();
	cvcs += pAdditionalCertificates;

	const auto* secureStorage = Env::getSingleton<SecureStorage>();
	mCvcChainBuilderProd = CVCertificateChainBuilder(cvcs + CVCertificate::fromHex(secureStorage->getCVRootCertificates(true)), true);
	mCvcChainBuilderTest = CVCertificateChainBuilder(cvcs + CVCertificate::fromHex(secureStorage->getCVRootCertificates(false)), false);
}
