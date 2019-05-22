/*!
 * \brief Implementation of \ref Reader for PCSC.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
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
		PcscReader(const QString& pReaderName);
		virtual ~PcscReader() override;

		Card* getCard() const override;

		SCARD_READERSTATE getState();

		bool hasFeature(FeatureID pFeatureID) const;

		/*!
		 * Check with hasFeature first for existence of the feature,
		 * otherwise this will cause an exception.
		 */
		PCSC_INT getFeatureValue(FeatureID pFeatureID);

		bool hasPaceCapability(PaceCapabilityId pPaceCapability);

};

} // namespace governikus
