/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "PcscUtils.h"

#include <QMap>

namespace governikus
{
defineEnumType(FeatureID,
		VERIFY_PIN_START = 0x1,
		VERIFY_PIN_FINISH = 0x2,
		MODIFY_PIN_START = 0x3,
		MODIFY_PIN_FINISH = 0x4,
		GET_KEY_PRESSED = 0x5,
		VERIFY_PIN_DIRECT = 0x6,
		MODIFY_PIN_DIRECT = 0x7,
		MCT_READERDIRECT = 0x8,
		MCT_UNIVERSAL = 0x9,
		IFD_PIN_PROP = 0xa,
		ABORT = 0xb,
		SET_SPE_MESSAGE = 0x0c,
		VERIFY_PIN_DIRECT_APP_ID = 0x0d,
		MODIFY_PIN_DIRECT_APP_ID = 0x0e,
		WRITE_DISPLAY = 0x0f,
		GET_KEY = 0x10,
		IFD_DISPLAY_PROPERTIES = 0x11,
		TLV_PROPERTIES = 0x12,
		CCID_ESC_COMMAND = 0x13,
		EXECUTE_PACE = 0x20)

class PcscReaderFeature
{
	private:
		QMap<FeatureID, PCSC_INT> mFeatures;

		friend QDebug operator<<(QDebug, const PcscReaderFeature&);

	public:
		PcscReaderFeature(const char* pFeaturesTLV, PCSC_INT pLength = 0);

		bool contains(FeatureID pFeatureID) const;

		/*!
		 * Check with contains first for existence of the
		 * feature, otherwise this will cause an exception.
		 */
		PCSC_INT getValue(FeatureID pFeatureID) const;
};


inline QDebug operator<<(QDebug pDbg, const governikus::PcscReaderFeature& pPcscReaderFeature)
{
	QDebugStateSaver saver(pDbg);
	pDbg << pPcscReaderFeature.mFeatures.keys();
	return pDbg;
}


} /* namespace governikus */
