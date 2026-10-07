/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "PcscReaderFeature.h"
#include "PcscUtils.h"
#include "Reader.h"

#include <QTimerEvent>


class test_PcscCard;


namespace governikus
{

class PcscCard;


class PcscReader
	: public Reader
{
	Q_OBJECT
	friend class ::test_PcscCard;

	private:
		SCARD_READERSTATE mReaderState;
		PcscReaderFeature mReaderFeatures;
		QScopedPointer<PcscCard> mPcscCard;

		SCARDCONTEXT mContextHandle;

		PCSC_RETURNCODE readReaderFeatures();

		[[nodiscard]] bool readCardStatus();

		void updateCard() override;

	public:
		explicit PcscReader(const QString& pReaderName);
		[[nodiscard]] PCSC_RETURNCODE init();
		~PcscReader() override;

		void printGetReaderInfo() const override;

		[[nodiscard]] Card* getCard() const override;

		[[nodiscard]] SCARD_READERSTATE getState() const;

		[[nodiscard]] bool hasFeature(FeatureID pFeatureID) const;

		/*!
		 * Check with hasFeature first for existence of the feature,
		 * otherwise this will cause an exception.
		 */
		[[nodiscard]] PCSC_INT getFeatureValue(FeatureID pFeatureID) const;


};

} // namespace governikus
