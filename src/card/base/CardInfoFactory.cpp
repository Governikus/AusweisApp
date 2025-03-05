/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "CardInfoFactory.h"

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

	const auto application = CardInfoFactory::detectCard(pCardConnectionWorker);

	if (application == FileRef())
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

	const CardInfo cardInfo(efCardAccess->getMobileEIDTypeInfo() ? CardType::SMART_EID : CardType::EID_CARD, application, efCardAccess);
	qCDebug(card) << "Card detected:" << cardInfo;
	return cardInfo;
}


bool CardInfoFactory::detectEid(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker, const FileRef& pRef)
{
	if (!pCardConnectionWorker->selectApplicationRoot(pRef))
	{
		return false;
	}

	if (pRef == FileRef::appPassport())
	{
		qCDebug(card) << "Passport found";
		return false;
	}

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


FileRef CardInfoFactory::detectCard(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
{
	for (const auto& appId : {FileRef::appEId(), FileRef::appPersosim(), FileRef::appPassport()})
	{
		if (detectEid(pCardConnectionWorker, appId))
		{
			return appId;
		}
	}

	return FileRef();
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
