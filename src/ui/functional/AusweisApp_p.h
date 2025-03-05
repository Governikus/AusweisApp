/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AusweisApp.h"

#include <QByteArray>

namespace governikus
{

void ausweisapp_init_internal(const QByteArray& pCmdline);
bool ausweisapp_is_running_internal();
void ausweisapp_started_internal();
AusweisAppCallback ausweisapp_get_callback_internal();
void ausweisapp_join_thread_internal();

} // namespace governikus
