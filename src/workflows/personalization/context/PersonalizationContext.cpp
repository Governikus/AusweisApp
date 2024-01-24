/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Smart context.
 */

#include "PersonalizationContext.h"

#include "LanguageLoader.h"

#include <QDate>


using namespace governikus;


PersonalizationContext::PersonalizationContext(const QString& pAppletServiceUrl)
	: AuthContext(Action::PERSONALIZATION)
	, mAllowSmartEidInstallation(true)
	, mSmartEidType(SmartEidType::UNKNOWN)
	, mChallengeType()
	, mFinalizeStatus(0)
	, mSessionIdentifier()
	, mChallenge()
	, mPreparePersonalizationData()
	, mNewPin()
	, mAppletServiceUrl(pAppletServiceUrl)
	, mLibVersion()
	, mRemainingAttempts(-1)
	, mRemainingDays(-1)
{
}


SmartEidType PersonalizationContext::getSmartEidType() const
{
	return mSmartEidType;
}


const QString& PersonalizationContext::getChallengeType() const
{
	return mChallengeType;
}


[[nodiscard]] const QString& PersonalizationContext::getLibVersion() const
{
	return mLibVersion;
}


void PersonalizationContext::setServiceInformation(SmartEidType pType, const QString& pChallengeType, const QString& pLibVersion)
{
	if (mSmartEidType != pType || mChallenge != pChallengeType || mLibVersion != pLibVersion)
	{
		mSmartEidType = pType;
		mChallengeType = pChallengeType;
		mLibVersion = pLibVersion;
		Q_EMIT fireServiceInformationChanged();
	}
}


int PersonalizationContext::getFinalizeStatus() const
{
	return mFinalizeStatus;
}


void PersonalizationContext::setFinalizeStatus(int pStatus)
{
	mFinalizeStatus = pStatus;
}


const QUuid& PersonalizationContext::getSessionIdentifier() const
{
	return mSessionIdentifier;
}


void PersonalizationContext::setSessionIdentifier(const QUuid& pSessionIdentifier)
{
	if (mSessionIdentifier != pSessionIdentifier)
	{
		mSessionIdentifier = pSessionIdentifier;
		Q_EMIT fireSessionIdentifierChanged();
	}
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


QString PersonalizationContext::getRestrictionDate() const
{
	QDate restrictionDate = QDate::currentDate();
	restrictionDate = restrictionDate.addDays(qAbs(mRemainingDays));
	const auto& usedLocale = LanguageLoader::getInstance().getUsedLocale();
	return usedLocale.toString(restrictionDate, QStringLiteral("d. MMMM yyyy"));
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


void PersonalizationContext::setProgress(int pProgress, const QString& pMessage, int pInitialValue, int pMaxValue)
{
	Q_ASSERT(pMaxValue > pInitialValue);

	// rewrite progress to combine two progresses from 0-100 in single progress bar.
	setProgress(pInitialValue + static_cast<int>((pMaxValue - pInitialValue) / 100.0 * pProgress), pMessage);
}
