/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ASN1TemplateUtil.h"


Q_DECLARE_LOGGING_CATEGORY(card)


const QLoggingCategory& governikus::getLoggingCategory()
{
	return card();
}

QByteArray governikus::getOpenSslError()
{
	QByteArrayList list;
	while (const auto errNum = ERR_get_error())
	{
		list += ERR_error_string(errNum, nullptr);
	}
	return list.join(QByteArrayLiteral(" | "));
}
