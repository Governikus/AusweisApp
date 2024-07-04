/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/CVCertificate.h"

namespace governikus
{

namespace CertificateHelper
{
QSharedPointer<const cvcertificate_st> fromHex(const QByteArray& pBytes);


} // namespace CertificateHelper

} // namespace governikus
