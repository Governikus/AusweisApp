/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardConnectionWorker.h"
#include "CardInfo.h"
#include "FileRef.h"

#include <QSharedPointer>

class test_CardInfoFactory;

namespace governikus
{

/*!
 * Factory for creation of CardInfo instances.
 */
class CardInfoFactory final
{
	friend class ::test_CardInfoFactory;

	public:
		/*!
		 * In order to create a CardInfo instance a connection is established to the smart card
		 * and  data is read.
		 */
		static CardInfo create(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

	private:
		static bool selectApplication(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker, const FileRef& pFileRef);

		/*!
		 * Checks, if the smart card is a german eID card (i.e. a NPA or an EAT) or a passport.
		 */
		static bool detectCard(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);
		static bool detectEid(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker, const FileRef& pRef);

		/*!
		 * Reads the EF.CardAccess
		 */
		static QSharedPointer<EFCardAccess> readEfCardAccess(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

		/*!
		 * According to TR-03105 we have to perform some checks on EF.CardAccess the first time we read it.
		 * Therefore we read it just once and handle malformed EF.CardAccess structures here in card recognition process.
		 */
		static bool checkEfCardAccess(const QSharedPointer<EFCardAccess>& pEfCardAccess);
};


} // namespace governikus
