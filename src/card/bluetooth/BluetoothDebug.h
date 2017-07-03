/*!
 * \brief Implementation of debug output for various Qt Bluetooth classes.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include <QBluetoothDeviceInfo>
#include <QDebug>

QDebug operator <<(QDebug pDbg, QBluetoothDeviceInfo::CoreConfigurations pConfig);
QDebug operator <<(QDebug pDbg, const QBluetoothDeviceInfo& pInfo);
