/*!
 * PcscReaderFeature.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "PcscReaderFeature.h"

#include <QLoggingCategory>
#include <QStringBuilder>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_pcsc)

namespace
{
template<typename T> static QString toStringListing(const T& pList)
{
	QStringList list;
	list.reserve(pList.size());
	for (auto entry : pList)
	{
		list += getEnumName(entry);
	}

	return QLatin1Char('[') % list.join(' ') % QLatin1Char(']');
}


}


const QMap<FeatureID, int>& PcscReaderFeature::getFeatures() const
{
	return mFeatures;
}


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
	for (; runner + 6 <= end; )
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

		int value = 0;
		value += *runner++ << 24 & 0xff000000;
		value += *runner++ << 16 & 0x00ff0000;
		value += *runner++ << 8 & 0x0000ff00;
		value += *runner++ << 0 & 0x000000ff;

		mFeatures.insert(fid, value);
	}
}


QString PcscReaderFeature::toString() const
{
	return toStringListing(mFeatures.keys());
}


QVector<PaceCapabilityId> PcscReaderPaceCapability::getPaceCapabilities() const
{
	return mPaceCapabilities;
}


PcscReaderPaceCapability::PcscReaderPaceCapability(const char* pCapabilitiesTLV, PCSC_INT pLength)
	: mPaceCapabilities()
{
	if (pCapabilitiesTLV == nullptr || pLength != 7)
	{
		qCDebug(card_pcsc) << "capabilities: null";
		return;
	}
	else if (pCapabilitiesTLV[0] != 0x00 || pCapabilitiesTLV[1] != 0x00 || pCapabilitiesTLV[2] != 0x00 || pCapabilitiesTLV[3] != 0x00)
	{
		qCWarning(card_pcsc) << "Error on determination of pace capabilities";
		return;
	}

	// in contrast to PCSC 10 Amendment 1: the output data of GetReaderPACECapabilities on Reiner SCT Konfort is of size 1!
	for (PaceCapabilityId capability : EnumPaceCapabilityId::getList())
	{
		if (pCapabilitiesTLV[6] & static_cast<char>(capability))
		{
			mPaceCapabilities.append(capability);
		}
	}
}


QString PcscReaderPaceCapability::toString() const
{
	return toStringListing(mPaceCapabilities);
}


#include "moc_PcscReaderFeature.cpp"
