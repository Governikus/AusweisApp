/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include <QByteArray>
#include <QDebug>

#pragma once

namespace governikus::privacy
{
QDebug logApdu(QDebug pDbg, const QByteArray& pApdu);
} // namespace governikus::privacy
