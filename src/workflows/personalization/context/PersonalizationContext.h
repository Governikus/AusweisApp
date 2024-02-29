/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"

#include <QString>
#include <QUuid>

#include <eid_applet_results.h>


namespace governikus
{

class SmartManager;

class PersonalizationContext
	: public AuthContext
{
	Q_OBJECT

	private:
		bool mAllowSmartEidInstallation;
		SmartEidType mSmartEidType;
		QString mChallengeType;
		int mFinalizeStatus;
		QUuid mSessionIdentifier;
		QString mChallenge;
		QString mPreparePersonalizationData;
		QString mNewPin;
		QString mBlockingCode;
		QString mAppletServiceUrl;
		QString mLibVersion;
		int mRemainingAttempts;
		int mRemainingDays;

	Q_SIGNALS:
		void fireNewPinChanged();
		void fireBlockingCodeChanged();
		void fireRemainingAttemptsChanged();
		void fireRemainingDaysChanged();
		void fireServiceInformationChanged();
		void fireSessionIdentifierChanged();

	public:
		explicit PersonalizationContext(const QString& pAppletServiceUrl);

		[[nodiscard]] SmartEidType getSmartEidType() const;
		[[nodiscard]] const QString& getChallengeType() const;
		[[nodiscard]] const QString& getLibVersion() const;
		void setServiceInformation(SmartEidType pType, const QString& pChallengeType, const QString& pLibVersion);

		[[nodiscard]] int getFinalizeStatus() const;
		void setFinalizeStatus(int pStatus);

		[[nodiscard]] const QUuid& getSessionIdentifier() const;
		void setSessionIdentifier(const QUuid& pSessionIdentifier);

		[[nodiscard]] QString getChallenge() const;
		void setChallenge(const QString& pChallenge);

		[[nodiscard]] const QString& getPreparePersonalizationData() const;
		void setPreparePersonalizationData(const QString& pPreparePersonalizationData);

		[[nodiscard]] const QString& getNewPin() const;
		void setNewPin(const QString& pNewPin);

		[[nodiscard]] const QString& getBlockingCode() const;
		void setBlockingCode(const QString& pBlockingCode);

		[[nodiscard]] int getRemainingAttempts() const;
		void setRemainingAttempts(int pRemainingAttempts);

		[[nodiscard]] QString getRestrictionDate() const;
		void setRemainingDays(int pRemainingDays);

		[[nodiscard]] QUrl getAppletServiceUrl(const QString& pArg) const;

		[[nodiscard]] QList<AcceptedEidType> getAcceptedEidTypes() const override;

		using WorkflowContext::setProgress;
		void setProgress(int pProgress, const QString& pMessage, int pInitialValue, int pMaxValue = 100);


};

} // namespace governikus
