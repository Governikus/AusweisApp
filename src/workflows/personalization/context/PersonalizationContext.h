/*!
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"

#include <QString>
#include <QUuid>


namespace governikus
{

class SmartManager;

class PersonalizationContext
	: public AuthContext
{
	Q_OBJECT

	private:
		bool mAllowSmartEidInstallation;
		QUuid mSessionIdentifier;
		QString mChallenge;
		QString mPreparePersonalizationData;
		QString mNewPin;
		QString mBlockingCode;
		QString mAppletServiceUrl;
		int mRemainingAttempts;
		int mRemainingDays;

	Q_SIGNALS:
		void fireNewPinChanged();
		void fireBlockingCodeChanged();
		void fireRemainingAttemptsChanged();
		void fireRemainingDaysChanged();

	public:
		explicit PersonalizationContext(const QString& pAppletServiceUrl);

		[[nodiscard]] bool allowSmartEidInstallation() const;
		void smartEidInstallationSuccessfull();

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

		[[nodiscard]] int getRemainingDays() const;
		void setRemainingDays(int pRemainingDays);

		[[nodiscard]] QUrl getAppletServiceUrl(const QString& pArg) const;

		[[nodiscard]] QVector<AcceptedEidType> getAcceptedEidTypes() const override;


};

} // namespace governikus
