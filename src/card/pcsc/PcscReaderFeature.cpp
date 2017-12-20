/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "PcscReaderFeature.h"

#include <QLoggingCategory>
#include <QStringBuilder>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_pcsc)


PcscReaderFeature::PcscReaderFeature(const char* pFeaturesTLV, PCSC_INT pLength)
	: mFeatures()
{
	if (pFeaturesTLV == nullptr)
	{
		qCDebug(card_pcsc) << "features: null";
		return;
	}

	const uchar* runner = reinterpret_cast<const uchar*>(pFeaturesTLV);
	const uchar* end = reinterpret_cast<const uchar*>(pFeaturesTLV + pLength);
	for (; runner + 6 <= end;)
	{
		if (!Enum<FeatureID>::isValue(*runner))
		{
			runner += 6;
			continue;
		}

		FeatureID fid = static_cast<FeatureID>(*runner);
		++runner;

		// skip length byte (always 1 byte : 0x04)
		++runner;

		PCSC_INT value = 0;
		value += static_cast<PCSC_INT>(*runner++) << 24 & 0xff000000;
		value += *runner++ << 16 & 0x00ff0000;
		value += *runner++ << 8 & 0x0000ff00;
		value += *runner++ << 0 & 0x000000ff;

		mFeatures.insert(fid, value);
	}
}


bool PcscReaderFeature::contains(FeatureID pFeatureID) const
{
	return mFeatures.contains(pFeatureID);
}


PCSC_INT PcscReaderFeature::getValue(FeatureID pFeatureID) const
{
	return mFeatures.find(pFeatureID).value();
}


#include "moc_PcscReaderFeature.cpp"
