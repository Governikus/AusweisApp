/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ASN1TemplateUtil.h"

QByteArray governikus::getOpenSslError()
{
	QByteArrayList list;
	while (const auto errNum = ERR_get_error())
	{
		list += ERR_error_string(errNum, nullptr);
	}
	return list.join(QByteArrayLiteral(" | "));
}
