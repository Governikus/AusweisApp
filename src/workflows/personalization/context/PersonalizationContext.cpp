/*!
 * \brief Smart context.
 *
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "PersonalizationContext.h"


using namespace governikus;


PersonalizationContext::PersonalizationContext(const QString& pAppletServiceUrl)
	: AuthContext(Action::PERSONALIZATION, QSharedPointer<ActivationContext>())
	, mAllowSmartEidInstallation(true)
	, mSessionIdentifier()
	, mChallenge()
	, mPreparePersonalizationData()
	, mNewPin()
	, mAppletServiceUrl(pAppletServiceUrl)
	, mRemainingAttempts(-1)
	, mRemainingDays(-1)
{
}


[[nodiscard]] bool PersonalizationContext::allowSmartEidInstallation() const
{
	return mAllowSmartEidInstallation;
}


void PersonalizationContext::smartEidInstallationSuccessfull()
{
	mAllowSmartEidInstallation = false;
}


const QUuid& PersonalizationContext::getSessionIdentifier() const
{
	return mSessionIdentifier;
}


void PersonalizationContext::setSessionIdentifier(const QUuid& pSessionIdentifier)
{
	mSessionIdentifier = pSessionIdentifier;
}


QString PersonalizationContext::getChallenge() const
{
	return mChallenge;
}


void PersonalizationContext::setChallenge(const QString& pChallenge)
{
	mChallenge = pChallenge;
}


const QString& PersonalizationContext::getPreparePersonalizationData() const
{
	return mPreparePersonalizationData;
}


void PersonalizationContext::setPreparePersonalizationData(const QString& pPreparePersonalizationData)
{
	mPreparePersonalizationData = pPreparePersonalizationData;
}


const QString& PersonalizationContext::getNewPin() const
{
	return mNewPin;
}


void PersonalizationContext::setNewPin(const QString& pNewPin)
{
	if (pNewPin != mNewPin)
	{
		mNewPin = pNewPin;
		Q_EMIT fireNewPinChanged();
	}
}


const QString& PersonalizationContext::getBlockingCode() const
{
	return mBlockingCode;
}


void PersonalizationContext::setBlockingCode(const QString& pBlockingCode)
{
	if (pBlockingCode != mBlockingCode)
	{
		mBlockingCode = pBlockingCode;
		Q_EMIT fireBlockingCodeChanged();
	}
}


int PersonalizationContext::getRemainingAttempts() const
{
	return mRemainingAttempts;
}


void PersonalizationContext::setRemainingAttempts(int pRemainingAttempts)
{
	if (pRemainingAttempts != mRemainingAttempts)
	{
		mRemainingAttempts = pRemainingAttempts;
		Q_EMIT fireRemainingAttemptsChanged();
	}
}


int PersonalizationContext::getRemainingDays() const
{
	return mRemainingDays;
}


void PersonalizationContext::setRemainingDays(int pRemainingDays)
{
	if (pRemainingDays != mRemainingDays)
	{
		mRemainingDays = pRemainingDays;
		Q_EMIT fireRemainingDaysChanged();
	}
}


QUrl PersonalizationContext::getAppletServiceUrl(const QString& pArg) const
{
	return QUrl(mAppletServiceUrl.arg(pArg));
}


QVector<AcceptedEidType> PersonalizationContext::getAcceptedEidTypes() const
{
	if (!getDidAuthenticateEac1())
	{
		return {AcceptedEidType::SE_CERTIFIED, AcceptedEidType::SE_ENDORSED, AcceptedEidType::HW_KEYSTORE};
	}

	return AuthContext::getAcceptedEidTypes();
}
