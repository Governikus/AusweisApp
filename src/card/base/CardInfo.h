/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "FileRef.h"
#include "SmartCardDefinitions.h"
#include "asn1/SecurityInfos.h"

#include <QCoreApplication>
#include <QSharedPointer>


namespace governikus
{
class CardInfo
{
	Q_DECLARE_TR_FUNCTIONS(governikus::CardInfo)
	friend class Reader;
	friend QDebug operator<<(QDebug, const CardInfo&);

	public:
		enum class TagType
		{
			UNKNOWN,
			NFC_4A,
			NFC_4B
		};

	private:
		CardType mCardType;
		FileRef mApplication;
		QSharedPointer<const EFCardAccess> mEfCardAccess;
		int mRetryCounter;
		bool mPinDeactivated;
		bool mPukInoperative;
		bool mPinInitial;
		TagType mTagType;
		static const int UNDEFINED_RETRY_COUNTER;

	public:
		CardInfo(CardType pCardType, const FileRef& pApplication = FileRef(), const QSharedPointer<const EFCardAccess>& = QSharedPointer<const EFCardAccess>(),
				int pRetryCounter = UNDEFINED_RETRY_COUNTER, bool pPinDeactivated = false, bool pPukInoperative = false, bool pPinInitial = false);

		void setCardType(CardType pCardType);
		[[nodiscard]] CardType getCardType() const;
		[[nodiscard]] QString getCardTypeString() const;

		[[nodiscard]] QSharedPointer<const EFCardAccess> getEfCardAccess() const;
		[[nodiscard]] MobileEidType getMobileEidType() const;

		[[nodiscard]] int getRetryCounter() const;
		void setRetryCounter(int pRetryCounter);

		[[nodiscard]] TagType getTagType() const;
		void setTagType(TagType pTagType);

		[[nodiscard]] const FileRef& getApplication() const;
		void setApplication(const FileRef& pApplication);

		[[nodiscard]] bool isRetryCounterDetermined() const;

		/*!
		 * The eID function has not been activated by the competent authority.
		 */
		[[nodiscard]] bool isPinDeactivated() const;

		/*!
		 * The PUK is inoperative. User is not able to unblock the PIN anymore.
		 */
		[[nodiscard]] bool isPukInoperative() const;

		[[nodiscard]] bool isPinInitial() const;
};

QDebug operator<<(QDebug pDbg, const CardInfo& pCardInfo);


} // namespace governikus
