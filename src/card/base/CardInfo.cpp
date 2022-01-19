/*!
 * \brief CardInfo holds smart card information, such as the type and some contained data structure (currently only the EF.CardAccess).
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "CardInfo.h"

#include "asn1/PaceInfo.h"
#include "asn1/SecurityInfos.h"
#include "CardConnectionWorker.h"
#include "SelectBuilder.h"

#include <QDebug>
#include <QLoggingCategory>
#include <QtGlobal>


Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


const int CardInfo::UNDEFINED_RETRY_COUNTER = -1;


CardInfo::CardInfo(CardType pCardType, const QSharedPointer<const EFCardAccess>& pEfCardAccess, int pRetryCounter, bool pPinDeactivated, bool pPukInoperative)
	: mCardType(pCardType)
	, mEfCardAccess(pEfCardAccess)
	, mRetryCounter(pRetryCounter)
	, mPinDeactivated(pPinDeactivated)
	, mPukInoperative(pPukInoperative)
{
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

		case CardType::PASSPORT:
			//: ERROR ALL_PLATFORMS A passport card is present/inserted. The text is only used in DiagnosisView.
			return tr("Passport");

		case CardType::EID_CARD:
			//: ERROR ALL_PLATFORMS An ID card is present/inserted. The text is only used in DiagnosisView.
			return tr("ID card (PA/eAT/eID)");
	}

	Q_UNREACHABLE();
	return QString();
}


bool CardInfo::isAvailable() const
{
	return mCardType != CardType::NONE;
}


bool CardInfo::isEid() const
{
	return mCardType == CardType::EID_CARD;
}


bool CardInfo::isPassport() const
{
	return mCardType == CardType::PASSPORT;
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


bool CardInfoFactory::create(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker, ReaderInfo& pReaderInfo)
{
	pReaderInfo.setCardInfo(CardInfo(CardType::UNKNOWN));

	if (pCardConnectionWorker == nullptr)
	{
		qCWarning(card) << "No connection to smart card";
		return false;
	}

	const CardType type = CardInfoFactory::detectCard(pCardConnectionWorker);

	if (type != CardType::EID_CARD)
	{
		qCWarning(card) << "Not a German EID card";
		return false;
	}

	const auto& efCardAccess = readEfCardAccess(pCardConnectionWorker);
	if (!checkEfCardAccess(efCardAccess))
	{
		qCWarning(card) << "EFCardAccess not found or is invalid";
		return false;
	}
	pReaderInfo.setCardInfo(CardInfo(type, efCardAccess));

	if (pCardConnectionWorker->updateRetryCounter() != CardReturnCode::OK)
	{
		qCWarning(card) << "Update of the retry counter failed";
		pReaderInfo.setCardInfo(CardInfo(CardType::UNKNOWN));
		return false;
	}

	return true;
}


bool CardInfoFactory::selectApplication(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker, const FileRef& pFileRef)
{
	qCDebug(card) << "Try to select application:" << pFileRef.path.toHex();

	CommandApdu command = SelectBuilder(pFileRef).build();
	ResponseApduResult select = pCardConnectionWorker->transmit(command);
	if (select.mResponseApdu.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "Cannot select application identifier:" << select.mResponseApdu.getReturnCode();
		return false;
	}

	return true;
}


bool CardInfoFactory::detectEid(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker, const FileRef& pRef)
{
	// 1. Select the application id
	selectApplication(pCardConnectionWorker, pRef);

	// 2. Select the master file
	CommandApdu command = SelectBuilder(FileRef::masterFile()).build();
	ResponseApduResult result = pCardConnectionWorker->transmit(command);
	if (result.mResponseApdu.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "Cannot select MF:" << result.mResponseApdu.getReturnCode();
		return false;
	}

	// 3. CL=00, INS=A4=SELECT, P1= 02, P2=0C, Lc=02, Data=2F00 (FI of EF.DIR), Le=absent
	command = SelectBuilder(FileRef::efDir()).build();
	result = pCardConnectionWorker->transmit(command);
	if (result.mResponseApdu.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "Cannot select EF.DIR:" << result.mResponseApdu.getReturnCode();
		return false;
	}

	// 4. CL=00, INS=B0=Read Binary, P1=00, P2=00 (no offset), Lc=00, Le=5A
	command = CommandApdu(QByteArray::fromHex("00B000005A"));
	result = pCardConnectionWorker->transmit(command);
	if (result.mResponseApdu.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "Cannot read EF.DIR:" << result.mResponseApdu.getReturnCode();
		return false;
	}

	// matching value from CIF
	static const QByteArray matchingValue = QByteArray::fromHex("61324F0FE828BD080FA000000167455349474E500F434941207A752044462E655369676E5100730C4F0AA000000167455349474E61094F07A0000002471001610B4F09E80704007F00070302610C4F0AA000000167455349474E");
	bool efDirMatches = result.mResponseApdu.getData().startsWith(matchingValue);
	if (!efDirMatches)
	{
		qCWarning(card) << "expected EF.DIR(00,5A): " << matchingValue.toHex();
		qCWarning(card) << "actual   EF.DIR(00,5A): " << result.mResponseApdu.getData().left(90).toHex();
		return false;
	}

	return true;
}


CardType CardInfoFactory::detectCard(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
{
	for (const auto& appId : {FileRef::appEId(), FileRef::appPersosim()})
	{
		if (detectEid(pCardConnectionWorker, appId))
		{
			return CardType::EID_CARD;
		}
	}

	if (selectApplication(pCardConnectionWorker, FileRef::appPassport()))
	{
		return CardType::PASSPORT;
	}

	return CardType::UNKNOWN;
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
				   << ", Pin deactivated: " << pCardInfo.mPinDeactivated << "}";
	// Skipping mEfCardAccess since there is no pretty formating available.

	return pDbg;
}


} // namespace governikus
