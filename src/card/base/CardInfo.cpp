/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief CardInfo holds smart card information, such as the type and some contained data structure (currently only the EF.CardAccess).
 */

#include "CardInfo.h"

#include "asn1/SecurityInfos.h"

#include <QDebug>
#include <QLoggingCategory>
#include <QtGlobal>


Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


const int CardInfo::UNDEFINED_RETRY_COUNTER = -1;


CardInfo::CardInfo(CardType pCardType, const QSharedPointer<const EFCardAccess>& pEfCardAccess, int pRetryCounter, bool pPinDeactivated, bool pPukInoperative, bool pPinInitial)
	: mCardType(pCardType)
	, mEfCardAccess(pEfCardAccess)
	, mRetryCounter(pRetryCounter)
	, mPinDeactivated(pPinDeactivated)
	, mPukInoperative(pPukInoperative)
	, mPinInitial(pPinInitial)
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
			//: ERROR ALL_PLATFORMS No card is present/inserted. The text is only used in DiagnosisView.
			return tr("not inserted", "Karte");

		case CardType::UNKNOWN:
			//: ERROR ALL_PLATFORMS An unknown card is present/inserted. The text is only used in DiagnosisView.
			return tr("unknown type", "Karte");

		case CardType::EID_CARD:
			//: ERROR ALL_PLATFORMS An ID card is present/inserted. The text is only used in DiagnosisView.
			return tr("ID card (PA/eAT/eID)");

		case CardType::SMART_EID:
			//: ERROR ALL_PLATFORMS A Smart-ID is present/inserted. The text is only used in DiagnosisView.
			return tr("Smart-eID");
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


bool CardInfo::isPinInitial() const
{
	return mPinInitial;
}


CardInfo::TagType CardInfo::getTagType() const
{
	return mTagType;
}


void CardInfo::setTagType(CardInfo::TagType pTagType)
{
	mTagType = pTagType;
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
				   << ", PIN deactivated: " << pCardInfo.mPinDeactivated
				   << ", PIN initial: " << pCardInfo.mPinInitial << "}";
	// Skipping mEfCardAccess since there is no pretty formatting available.

	return pDbg;
}


} // namespace governikus
