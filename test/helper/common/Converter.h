/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/CVCertificate.h"

#include <QByteArray>
#include <openssl/bn.h>


namespace governikus
{

namespace Converter
{
QSharedPointer<const cvcertificate_st> certificatefromHex(const QByteArray& pBytes);

QByteArray toHex(const BIGNUM* pBigNum);


} // namespace Converter

} // namespace governikus
