/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "LogPrivacy.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(secure)

QDebug privacy::logApdu(QDebug pDbg, const QByteArray& pApdu)
{
	QDebugStateSaver saver(pDbg);

	constexpr auto START = 5;
	constexpr auto END = 2;
	if (secure().isDebugEnabled() || pApdu.size() < (START + END))
	{
		pDbg << pApdu.toHex();
	}
	else
	{
		pDbg.noquote().nospace() << '"'
								 << pApdu.first(START).toHex()
								 << '~'
								 << pApdu.last(END).toHex()
								 << "\" ("
								 << pApdu.size()
								 << ')';
	}

	return pDbg;
}
