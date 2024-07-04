/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "CertificateHelper.h"

using namespace governikus;

QSharedPointer<const CVCertificate> CertificateHelper::fromHex(const QByteArray& pBytes)
{
	return CVCertificate::fromRaw(QByteArray::fromHex(pBytes));
}
