/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for checking the ID card in "playground".
 */

#pragma once

#include "ReaderInfo.h"

#include <QObject>


namespace governikus
{

class CheckIDCardModel
	: public QObject
{
	Q_OBJECT

	Q_PROPERTY(CheckIDCardResult result READ getResult NOTIFY fireResultChanged)

	public:
		enum class CheckIDCardResult
		{
			UNKNOWN,
			NO_NFC,
			CARD_NOT_DETECTED,
			UNKNOWN_CARD_DETECTED,
			ID_CARD_DETECTED,
			INSUFFICIENT_APDU_LENGTH,
			CARD_ACCESS_FAILED,
			PIN_DEACTIVATED,
			PIN_SUSPENDED,
			PIN_BLOCKED,
			SUCCESS
		};
		Q_ENUM(CheckIDCardResult)

		explicit CheckIDCardModel(QObject* pParent = nullptr);
		~CheckIDCardModel() override;

		Q_INVOKABLE void startScan();
		Q_INVOKABLE void startScanIfNecessary();
		Q_INVOKABLE void stopScan();

		[[nodiscard]] CheckIDCardResult getResult() const;

	private:
		bool mIsRunning;
		CheckIDCardResult mResult;
		QString mReaderWithCard;

	private Q_SLOTS:
		void onCardInserted(const ReaderInfo& pInfo);
		void onCardRemoved(const ReaderInfo& pInfo);
		void onReaderAdded(const ReaderInfo& pInfo);
		void onReaderRemoved(const ReaderInfo& pInfo);
		void onReaderPropertiesUpdated(const ReaderInfo& pInfo);

		void stopScanWithResult(CheckIDCardResult result);

	Q_SIGNALS:
		void fireResultChanged();
		void fireScanCompleted();
};

} // namespace governikus
