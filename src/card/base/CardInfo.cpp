/*!
 * \brief CardInfo holds smart card information, such as the type and some contained data structure (currently only the EF.CardAccess).
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "CardInfo.h"

#include "asn1/PACEInfo.h"
#include "asn1/SecurityInfos.h"
#include "CardConnectionWorker.h"

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
			return tr("not inserted", "Karte");

		case CardType::UNKNOWN:
			return tr("unknown type", "Karte");

		case CardType::EID_CARD:
			return tr("ID card (PA/eAT)");
	}

	Q_UNREACHABLE();
}


bool CardInfo::isAvailable() const
{
	return mCardType != CardType::NONE;
}


bool CardInfo::isEid() const
{
	return mCardType == CardType::EID_CARD;
}


QSharedPointer<const EFCardAccess> CardInfo::getEfCardAccess() const
{
	return mEfCardAccess;
}


QString CardInfo::getEidApplicationPath() const
{
	return mCardType == CardType::EID_CARD ? QStringLiteral("e80704007f00070302") : QString();
}


int CardInfo::getRetryCounter() const
{
	return mRetryCounter;
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
	if (pCardConnectionWorker == nullptr)
	{
		qCWarning(card) << "No connection to smart card";
		pReaderInfo.setCardInfo(CardInfo(CardType::UNKNOWN));
		return false;
	}

	if (!CardInfoFactory::isGermanEidCard(pCardConnectionWorker))
	{
		qCWarning(card) << "Not a German EID card";
		pReaderInfo.setCardInfo(CardInfo(CardType::UNKNOWN));
		return false;
	}

	QSharedPointer<EFCardAccess> efCardAccess = readEfCardAccess(pCardConnectionWorker);
	if (efCardAccess == nullptr || !checkEfCardAccess(efCardAccess))
	{
		qCWarning(card) << "EFCardAccess not found or invalid";
		pReaderInfo.setCardInfo(CardInfo(CardType::UNKNOWN));
		return false;
	}

	pReaderInfo.setCardInfo(CardInfo(CardType::EID_CARD, efCardAccess));
	pCardConnectionWorker->updateRetryCounter();
	return true;
}


bool CardInfoFactory::isGermanEidCard(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
{
	// This is actually not specified in the CIF, but we do it to make the PersoSim work
	// 0. Select the master file
	CommandApdu command = SelectBuilder(FileRef::masterFile()).build();
	ResponseApdu response;
	CardReturnCode returnCode = pCardConnectionWorker->transmit(command, response);
	if (returnCode != CardReturnCode::OK || response.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "Cannot select MF";

		if (returnCode != CardReturnCode::OK)
		{
			qCDebug(card) << "CardConnectionWorker return code" << returnCode;
		}

		if (response.getReturnCode() != StatusCode::SUCCESS)
		{
			qCDebug(card) << "ResponseApdu return code" << response.getReturnCode();
		}

		return false;
	}

	// 1. CL=00, INS=A4=SELECT, P1= 02, P2=0C, Lc=02, Data=2F00 (FI of EF.DIR), Le=absent
	command = SelectBuilder(FileRef::efDir()).build();
	returnCode = pCardConnectionWorker->transmit(command, response);
	if (returnCode != CardReturnCode::OK || response.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "Cannot select EF.DIR";
		return false;
	}

	// 2. CL=00, INS=B0=Read Binary, P1=00, P2=00 (no offset), Lc=00, Le=5A
	command = CommandApdu(QByteArray::fromHex("00B000005A"));
	returnCode = pCardConnectionWorker->transmit(command, response);
	if (returnCode != CardReturnCode::OK || response.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "Cannot read EF.DIR";
		return false;
	}

	// matching value from CIF
	static const QByteArray matchingValue = QByteArray::fromHex("61324F0FE828BD080FA000000167455349474E500F434941207A752044462E655369676E5100730C4F0AA000000167455349474E61094F07A0000002471001610B4F09E80704007F00070302610C4F0AA000000167455349474E");
	bool efDirMatches = response.getData().startsWith(matchingValue);
	if (!efDirMatches)
	{
		qCWarning(card) << "expected EF.DIR(00,5A): " << matchingValue.toHex();
		qCWarning(card) << "actual   EF.DIR(00,5A): " << response.getData().left(90).toHex();
		return false;
	}

	return true;
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
	/*
	 * At least one PACEInfo must have standardized domain parameters
	 */
	bool containsStandardizedDomainParameters = false;
	const auto& infos = pEfCardAccess->getPACEInfos();
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


}
