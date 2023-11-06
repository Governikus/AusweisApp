/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "CardInfoFactory.h"

#include "apdu/FileCommand.h"
#include "asn1/ApplicationTemplates.h"
#include "asn1/PaceInfo.h"

#include <QDebug>
#include <QLoggingCategory>
#include <QtGlobal>


Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


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
