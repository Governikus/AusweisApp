/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "NfcCard.h"
#include "Reader.h"

#include <QNearFieldManager>
#include <QQueue>


class test_NfcReader;


namespace governikus
{

class NfcReader
	: public ConnectableReader
{
	Q_OBJECT
	friend class ::test_NfcReader;

	private:
		QNearFieldManager mNfManager;
		QQueue<QNearFieldTarget*> mTargetQueue;
		QScopedPointer<NfcCard, QScopedPointerDeleteLater> mCard;

	Q_SIGNALS:
		void fireNfcAdapterStateChanged(bool pEnabled);

	private Q_SLOTS:
		void adapterStateChanged(QNearFieldManager::AdapterState pState);
		void targetDetected(QNearFieldTarget* pTarget);
		void targetLost(const QNearFieldTarget* pTarget);
		void setProgressMessage(const QString& pMessage);

	public:
		NfcReader();
		~NfcReader() override;

		[[nodiscard]] bool isEnabled() const;
		[[nodiscard]] Card* getCard() const override;

		void connectReader() override;
		void disconnectReader(const QString& pError) override;
};

} // namespace governikus
