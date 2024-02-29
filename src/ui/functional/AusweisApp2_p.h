/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AusweisApp2.h"

#include <QByteArray>

namespace governikus
{

void ausweisapp2_init_internal(const QByteArray& pCmdline);
bool ausweisapp2_is_running_internal();
void ausweisapp2_started_internal();
AusweisApp2Callback ausweisapp2_get_callback_internal();
void ausweisapp2_join_thread_internal();

} // namespace governikus
