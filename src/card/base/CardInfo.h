/*!
 * CardInfo.h
 *
 * \brief Contains the CardInfo and the CardInfoFactory
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "SmartCardDefinitions.h"
#include "asn1/SecurityInfos.h"

#include <QSharedPointer>

namespace governikus
{

class CardConnectionWorker;
class PACEInfo;
class Reader;
class ReaderInfo;

/*!
 * Holds smart card informations.
 * An instance of CardInfo is created using the CardInfoFactory.
 */
class CardInfo
{
	CardType mCardType;
	QSharedPointer<const EFCardAccess> mEfCardAccess;
	int mRetryCounter;
	bool mPinDeactivated;

	public:
		CardInfo(CardType pCardType, QSharedPointer<const EFCardAccess> = QSharedPointer<const EFCardAccess>(), int pRetryCounter = -1, bool pPinDeactivated = false);

		CardType getCardType() const;

		QSharedPointer<const EFCardAccess> getEfCardAccess() const;

		QString getEidApplicationPath() const;

		int getRetryCounter() const;

		/*!
		 * The online identification function has not been activated by the competent authority.
		 */
		bool isPinDeactivated() const;

		friend class Reader;
};


/*!
 * Factory for creation of CardInfo instances.
 */
class CardInfoFactory
{
	public:
		/*!
		 * In order to create a CardInfo instance a connection is established to the smart card
		 * and  data is read.
		 */
		static bool create(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker, ReaderInfo& pReaderInfo);

	private:
		/*!
		 * Checks, if the smart card is a german eID card, i.e. a NPA or an EAT.
		 */
		static bool isGermanEidCard(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

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


} /* namespace governikus */
