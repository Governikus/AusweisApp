/**
 * Copyright (c) 2024-2026 Governikus Service GmbH, Germany
 */

#include <QByteArray>
#include <QDebug>

#pragma once

namespace governikus::privacy
{
QDebug logApdu(QDebug pDbg, const QByteArray& pApdu);
} // namespace governikus::privacy
