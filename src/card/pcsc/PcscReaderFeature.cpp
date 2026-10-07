/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "PcscReaderFeature.h"

#include <QLoggingCategory>
#include <QStringBuilder>
#include <QtEndian>


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

		mFeatures.insert(fid, qFromBigEndian<quint32>(runner));
		runner += 4;
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
