/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#include "CardInfo.h"

#include "asn1/SecurityInfos.h"

#include <QDebug>
#include <QLoggingCategory>
#include <QtGlobal>


Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


const int CardInfo::UNDEFINED_RETRY_COUNTER = -1;


CardInfo::CardInfo(CardType pCardType, const FileRef& pApplication, const QSharedPointer<const EFCardAccess>& pEfCardAccess, int pRetryCounter, bool pPinDeactivated, bool pPukInoperative)
	: mCardType(pCardType)
	, mApplication(pApplication)
	, mEfCardAccess(pEfCardAccess)
	, mRetryCounter(pRetryCounter)
	, mPinDeactivated(pPinDeactivated)
	, mPukInoperative(pPukInoperative)
	, mTagType(TagType::UNKNOWN)
{
}


void CardInfo::setCardType(CardType pCardType)
{
	mCardType = pCardType;
}


CardType CardInfo::getCardType() const
{
	return mCardType;
}


QString CardInfo::getCardTypeString() const
{
	switch (mCardType)
	{
		case CardType::NONE:
			//: ALL_PLATFORMS No card is present/inserted. The text is only used in DiagnosisView.
			return tr("not inserted", "Karte");

		case CardType::UNKNOWN:
			//: ALL_PLATFORMS An unknown card is present/inserted. The text is only used in DiagnosisView.
			return tr("unknown type", "Karte");

		case CardType::EID_CARD:
			//: ALL_PLATFORMS An ID card is present/inserted. The text is only used in DiagnosisView.
			return tr("ID card (PA/eAT/eID)");
	}

	Q_UNREACHABLE();
}


QSharedPointer<const EFCardAccess> CardInfo::getEfCardAccess() const
{
	return mEfCardAccess;
}


int CardInfo::getRetryCounter() const
{
	return mRetryCounter;
}


void CardInfo::setRetryCounter(int pRetryCounter)
{
	mRetryCounter = pRetryCounter;
}


bool CardInfo::isRetryCounterDetermined() const
{
	return mRetryCounter != UNDEFINED_RETRY_COUNTER;
}


bool CardInfo::isPinDeactivated() const
{
	return mPinDeactivated;
}


bool CardInfo::isPukInoperative() const
{
	return mPukInoperative;
}


CardInfo::TagType CardInfo::getTagType() const
{
	return mTagType;
}


void CardInfo::setTagType(CardInfo::TagType pTagType)
{
	mTagType = pTagType;
}


const FileRef& CardInfo::getApplication() const
{
	return mApplication;
}


void CardInfo::setApplication(const FileRef& pApplication)
{
	mApplication = pApplication;
}


MobileEidType CardInfo::getMobileEidType() const
{
	if (!mEfCardAccess || !mEfCardAccess->getMobileEIDTypeInfo())
	{
		return MobileEidType::UNKNOWN;
	}

	const auto oid = mEfCardAccess->getMobileEIDTypeInfo()->getOid();
	if (oid == Oid(KnownOid::ID_MOBILE_EID_TYPE_SE_CERTIFIED))
	{
		return MobileEidType::SE_CERTIFIED;
	}
	if (oid == Oid(KnownOid::ID_MOBILE_EID_TYPE_SE_ENDORSED))
	{
		return MobileEidType::SE_ENDORSED;
	}
	if (oid == Oid(KnownOid::ID_MOBILE_EID_TYPE_HW_KEYSTORE))
	{
		return MobileEidType::HW_KEYSTORE;
	}

	return MobileEidType::UNKNOWN;
}


namespace governikus
{

QDebug operator<<(QDebug pDbg, const CardInfo& pCardInfo)
{
	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << "{Type: " << pCardInfo.mCardType
				   << ", Retry counter: " << pCardInfo.mRetryCounter
				   << ", PIN deactivated: " << pCardInfo.mPinDeactivated << "}";
	// Skipping mEfCardAccess since there is no pretty formatting available.

	return pDbg;
}


} // namespace governikus
