/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "PcscReaderFeature.h"

#include <QLoggingCategory>
#include <QStringBuilder>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_pcsc)


PcscReaderFeature::PcscReaderFeature(const QByteArray& pFeaturesTLV)
	: mFeatures()
{
	if (pFeaturesTLV.isEmpty())
	{
		qCDebug(card_pcsc) << "features: null";
		return;
	}

	const auto* const end = reinterpret_cast<const uchar*>(pFeaturesTLV.constData() + pFeaturesTLV.size());
	for (const auto* runner = reinterpret_cast<const uchar*>(pFeaturesTLV.constData()); runner + 6 <= end;)
	{
		if (!Enum<FeatureID>::isValue(*runner))
		{
			runner += 6;
			continue;
		}

		auto fid = static_cast<FeatureID>(*runner);
		++runner;

		// skip length byte (always 1 byte : 0x04)
		++runner;

		PCSC_INT value = 0;
		value += static_cast<PCSC_INT>(*runner++) << 24 & 0xFF000000;
		value += *runner++ << 16 & 0x00FF0000;
		value += *runner++ << 8 & 0x0000FF00;
		value += *runner++ << 0 & 0x000000FF;

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
