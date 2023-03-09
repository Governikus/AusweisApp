/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief CardInfo holds smart card information, such as the type and some contained data structure (currently only the EF.CardAccess).
 */

#include "CardInfo.h"

#include "CardConnectionWorker.h"
#include "apdu/FileCommand.h"
#include "asn1/ApplicationTemplates.h"
#include "asn1/PaceInfo.h"
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


CardInfo CardInfoFactory::create(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
{
	if (pCardConnectionWorker == nullptr)
	{
		qCWarning(card) << "No connection to smart card";
		return CardInfo(CardType::UNKNOWN);
	}

	if (!CardInfoFactory::detectCard(pCardConnectionWorker))
	{
		qCWarning(card) << "Not a German EID card";
		return CardInfo(CardType::UNKNOWN);
	}

	const auto& efCardAccess = readEfCardAccess(pCardConnectionWorker);
	if (!checkEfCardAccess(efCardAccess))
	{
		qCWarning(card) << "EFCardAccess not found or is invalid";
		return CardInfo(CardType::UNKNOWN);
	}

	const CardInfo cardInfo(efCardAccess->getMobileEIDTypeInfo() ? CardType::SMART_EID : CardType::EID_CARD, efCardAccess);
	qCDebug(card) << "Card detected:" << cardInfo;
	return cardInfo;
}


bool CardInfoFactory::selectApplication(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker, const FileRef& pFileRef)
{
	qCDebug(card) << "Try to select application:" << pFileRef;

	FileCommand command(pFileRef);
	ResponseApduResult select = pCardConnectionWorker->transmit(command);
	if (select.mResponseApdu.getStatusCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "Cannot select application identifier:" << select.mResponseApdu.getStatusCode();
		return false;
	}

	return true;
}


bool CardInfoFactory::detectEid(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker, const FileRef& pRef)
{
	// 1. Select the application id
	selectApplication(pCardConnectionWorker, pRef);

	// 2. Select the master file
	FileCommand command(FileRef::masterFile());
	ResponseApduResult result = pCardConnectionWorker->transmit(command);
	if (result.mResponseApdu.getStatusCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "Cannot select MF:" << result.mResponseApdu.getStatusCode();
		return false;
	}

	// 3. Read EF.DIR
	QByteArray rawEfDir;
	if (pCardConnectionWorker->readFile(FileRef::efDir(), rawEfDir) != CardReturnCode::OK)
	{
		qCWarning(card) << "Cannot read EF.DIR";
		return false;
	}

	const auto efDir = ApplicationTemplates::decode(rawEfDir);
	if (efDir.isNull())
	{
		qCWarning(card) << "Cannot parse EF.DIR";
		return false;
	}

	if (!efDir->contains(FileRef::appEId().getIdentifier()))
	{
		qCWarning(card) << "EF.DIR does not match:" << rawEfDir.toHex();
		return false;
	}

	return true;
}


bool CardInfoFactory::detectCard(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
{
	for (const auto& appId : {FileRef::appEId(), FileRef::appPersosim()})
	{
		const auto eidAvailable = detectEid(pCardConnectionWorker, appId);
		if (eidAvailable)
		{
			return true;
		}
	}

	if (selectApplication(pCardConnectionWorker, FileRef::appPassport()))
	{
		qCDebug(card) << "Passport found";
	}

	return false;
}


QSharedPointer<EFCardAccess> CardInfoFactory::readEfCardAccess(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
{
	QByteArray efCardAccessBytes;
	if (pCardConnectionWorker->readFile(FileRef::efCardAccess(), efCardAccessBytes) != CardReturnCode::OK)
	{
		qCCritical(card) << "Error while reading EF.CardAccess: Cannot read EF.CardAccess";
		return QSharedPointer<EFCardAccess>();
	}

	auto efCardAccess = EFCardAccess::decode(efCardAccessBytes);
	if (efCardAccess == nullptr)
	{
		qCCritical(card) << "Error while reading EF.CardAccess: Cannot parse EFCardAccess";
	}
	return efCardAccess;
}


bool CardInfoFactory::checkEfCardAccess(const QSharedPointer<EFCardAccess>& pEfCardAccess)
{
	if (!pEfCardAccess)
	{
		return false;
	}

	/*
	 * At least one PACEInfo must have standardized domain parameters
	 */
	bool containsStandardizedDomainParameters = false;
	const auto& infos = pEfCardAccess->getPaceInfos();
	for (const auto& paceInfo : infos)
	{
		if (paceInfo->isStandardizedDomainParameters())
		{
			containsStandardizedDomainParameters = true;
			break;
		}
	}
	if (!containsStandardizedDomainParameters)
	{
		qCCritical(card) << "Error while reading EF.CardAccess: No PACEInfo with standardized domain parameters found";
		return false;
	}

	return true;
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
