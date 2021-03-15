/*!
 * \brief Contains the CardInfo and the CardInfoFactory
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/SecurityInfos.h"
#include "FileRef.h"
#include "SmartCardDefinitions.h"

#include <QCoreApplication>
#include <QSharedPointer>


namespace governikus
{

class CardConnectionWorker;
class Reader;
class ReaderInfo;

/*!
 * Holds smart card informations.
 * An instance of CardInfo is created using the CardInfoFactory.
 */
class CardInfo
{
	Q_DECLARE_TR_FUNCTIONS(governikus::CardInfo)

	private:
		CardType mCardType;
		QSharedPointer<const EFCardAccess> mEfCardAccess;
		int mRetryCounter;
		bool mPinDeactivated;
		bool mPukInoperative;
		static const int UNDEFINED_RETRY_COUNTER;

		friend QDebug operator<<(QDebug, const CardInfo&);

	public:
		CardInfo(CardType pCardType, const QSharedPointer<const EFCardAccess>& = QSharedPointer<const EFCardAccess>(),
				int pRetryCounter = UNDEFINED_RETRY_COUNTER, bool pPinDeactivated = false, bool pPukInoperative = false);

		[[nodiscard]] QString getCardTypeString() const;
		[[nodiscard]] bool isAvailable() const;
		[[nodiscard]] bool isEid() const;
		[[nodiscard]] bool isPassport() const;

		[[nodiscard]] QSharedPointer<const EFCardAccess> getEfCardAccess() const;

		[[nodiscard]] int getRetryCounter() const;
		void setRetryCounter(int pRetryCounter);

		[[nodiscard]] bool isRetryCounterDetermined() const;

		/*!
		 * The online identification function has not been activated by the competent authority.
		 */
		[[nodiscard]] bool isPinDeactivated() const;

		/*!
		 * The PUK is inoperative. User is not able to unblock the PIN anymore.
		 */
		[[nodiscard]] bool isPukInoperative() const;

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
		static bool selectApplication(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker, const FileRef& pFileRef);

		/*!
		 * Checks, if the smart card is a german eID card (i.e. a NPA or an EAT) or a passport.
		 */
		static CardType detectCard(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

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


QDebug operator<<(QDebug pDbg, const CardInfo& pCardInfo);


} // namespace governikus
