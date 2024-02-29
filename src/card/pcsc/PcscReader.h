/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of \ref Reader for PCSC.
 */

#pragma once

#include "PcscReaderFeature.h"
#include "PcscUtils.h"
#include "Reader.h"

#include <QTimerEvent>


namespace governikus
{

class PcscCard;


class PcscReader
	: public Reader
{
	Q_OBJECT

	private:
		SCARD_READERSTATE mReaderState;
		PcscReaderFeature mReaderFeatures;
		QScopedPointer<PcscCard> mPcscCard;

		SCARDCONTEXT mContextHandle;

		void fetchGetReaderInfo();
		PCSC_RETURNCODE readReaderFeatures();

		void updateCard() override;

	public:
		explicit PcscReader(const QString& pReaderName);
		[[nodiscard]] PCSC_RETURNCODE init();
		~PcscReader() override;

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
