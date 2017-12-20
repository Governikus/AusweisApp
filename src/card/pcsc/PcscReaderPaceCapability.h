/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "PcscUtils.h"

#include <QMap>

namespace governikus
{
defineEnumType(PaceCapabilityId,
		ESIGN = 0x10,
		EID = 0x20,
		GENERIC = 0x40,
		DESTROY_CHANNEL = 0x80)

class PcscReaderPaceCapability
{
	private:
		QVector<PaceCapabilityId> mPaceCapabilities;

		friend QDebug operator<<(QDebug, const PcscReaderPaceCapability&);

	public:
		PcscReaderPaceCapability(const char* pCapabilitiesTLV, PCSC_INT pLength = 0);

		bool contains(PaceCapabilityId pPaceCapabilityId) const;
};


inline QDebug operator<<(QDebug pDbg, const governikus::PcscReaderPaceCapability& pPcscReaderPaceCapability)
{
	QDebugStateSaver saver(pDbg);
	pDbg << pPcscReaderPaceCapability.mPaceCapabilities.toList();
	return pDbg;
}


} /* namespace governikus */
