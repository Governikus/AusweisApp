/*!
 * \brief Implementation of \ref Reader for NFC.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "NfcCard.h"
#include "Reader.h"

namespace governikus
{

class NfcReader
	: public Reader
{
	Q_OBJECT

	private:
		QScopedPointer<NfcCard, QScopedPointerDeleteLater> mCard;

		virtual CardEvent updateCard() override;

	private Q_SLOTS:
		void onCardRemoved();

	public:
		NfcReader();
		virtual ~NfcReader();

		virtual Card* getCard() const override;
};

} /* namespace governikus */
