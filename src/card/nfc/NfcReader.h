/*!
 * \brief Implementation of \ref Reader for NFC.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "NfcCard.h"
#include "Reader.h"

#include <QNearFieldManager>


namespace governikus
{

class NfcReader
	: public Reader
{
	Q_OBJECT

	private:
		QNearFieldManager mNfManager;
		QScopedPointer<NfcCard, QScopedPointerDeleteLater> mCard;

		virtual CardEvent updateCard() override;

	Q_SIGNALS:
		void fireNfcAdapterStateChanged(bool pEnabled);

	private Q_SLOTS:
		void targetDetected(QNearFieldTarget* pTarget);
		void targetLost(QNearFieldTarget* pTarget);

	public:
		NfcReader();
		virtual ~NfcReader() override;

		virtual Card* getCard() const override;
};

} // namespace governikus
