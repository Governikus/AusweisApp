/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "PcscUtils.h"

using namespace governikus;


QString pcsc::toString(PCSC_RETURNCODE pCode)
{
	const auto& metaEnum = QMetaEnum::fromType<PcscReturnCode>();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 9, 0))
	const char* const name = metaEnum.valueToKey(static_cast<quint64>(pCode));
#else
	const char* const name = metaEnum.valueToKey(static_cast<int>(pCode));
#endif
	if (Q_UNLIKELY(name == nullptr))
	{
		return QStringLiteral("UNKNOWN_STATE (%1)").arg(pCode, 8, 16, QLatin1Char('0'));
	}

	return QString::fromLatin1(name);
}


QDataStream& pcsc::operator<<(QDataStream& pStream, const PcscReturnCode& pCode)
{
	return pStream << static_cast<qint64>(pCode);
}


QDataStream& pcsc::operator>>(QDataStream& pStream, PcscReturnCode& pCode)
{
	qint64 tmp;
	pStream >> tmp;
	pCode = static_cast<PcscReturnCode>(tmp);
	return pStream;
}
