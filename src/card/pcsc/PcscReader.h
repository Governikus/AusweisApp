/*!
 * \brief Implementation of \ref Reader for PCSC.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "PcscReaderFeature.h"
#include "PcscReaderPaceCapability.h"
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
		PcscReaderPaceCapability mPaceCapabilities;
		QScopedPointer<PcscCard> mPcscCard;

		SCARDCONTEXT mContextHandle;

		PCSC_RETURNCODE readReaderFeaturesAndPACECapabilities();

		virtual Reader::CardEvent updateCard() override;

	public:
		explicit PcscReader(const QString& pReaderName);
		virtual ~PcscReader() override;

		Card* getCard() const override;

		SCARD_READERSTATE getState() const;

		bool hasFeature(FeatureID pFeatureID) const;

		/*!
		 * Check with hasFeature first for existence of the feature,
		 * otherwise this will cause an exception.
		 */
		PCSC_INT getFeatureValue(FeatureID pFeatureID) const;

		bool hasPaceCapability(PaceCapabilityId pPaceCapability) const;

};

} // namespace governikus
