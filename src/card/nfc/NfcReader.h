/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of \ref Reader for NFC.
 */

#pragma once

#include "NfcCard.h"
#include "Reader.h"

#include <QNearFieldManager>


namespace governikus
{

class NfcReader
	: public ConnectableReader
{
	Q_OBJECT

	private:
		QNearFieldManager mNfManager;
		QScopedPointer<NfcCard, QScopedPointerDeleteLater> mCard;

	Q_SIGNALS:
		void fireNfcAdapterStateChanged(bool pEnabled);

	private Q_SLOTS:
		void adapterStateChanged(QNearFieldManager::AdapterState pState);
		void targetDetected(QNearFieldTarget* pTarget);
		void targetLost(QNearFieldTarget* pTarget);
		void setProgressMessage(const QString& pMessage);

	public:
		NfcReader();
		~NfcReader() override;

		[[nodiscard]] bool isEnabled() const;
		[[nodiscard]] Card* getCard() const override;

		void connectReader() override;
		void disconnectReader(const QString& pError = QString()) override;
};

} // namespace governikus
