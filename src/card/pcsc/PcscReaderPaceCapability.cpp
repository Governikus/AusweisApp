/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "PcscReaderPaceCapability.h"

#include <QLoggingCategory>
#include <QStringBuilder>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_pcsc)


PcscReaderPaceCapability::PcscReaderPaceCapability(const char* const pCapabilitiesTLV, PCSC_INT pLength)
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
	const auto list = Enum<PaceCapabilityId>::getList();
	for (PaceCapabilityId capability : list)
	{
		if (pCapabilitiesTLV[6] & static_cast<char>(capability))
		{
			mPaceCapabilities += capability;
		}
	}
}


bool PcscReaderPaceCapability::contains(PaceCapabilityId pPaceCapabilityId) const
{
	return mPaceCapabilities.contains(pPaceCapabilityId);
}


#include "moc_PcscReaderPaceCapability.cpp"
