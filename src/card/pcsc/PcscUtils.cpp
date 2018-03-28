/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "PcscUtils.h"

using namespace governikus;


QString PcscUtils::toString(PCSC_RETURNCODE pCode)
{
	const auto& metaEnum = QMetaEnum::fromType<PcscReturnCode>();
	const char* name = metaEnum.valueToKey(static_cast<int>(pCode));
	if (Q_UNLIKELY(name == nullptr))
	{
		return QStringLiteral("UNKNOWN_STATE (%1)").arg(pCode, 8, 16, QLatin1Char('0'));
	}

	return QString::fromLatin1(name);
}
