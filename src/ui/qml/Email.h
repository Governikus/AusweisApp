/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"

#include <QString>
#include <QUrl>


namespace governikus
{

class GlobalStatus;

QString generateMailBody(const GlobalStatus& pStatus, const QUrl& pServiceUrl = {}, bool pPercentEncoding = false, bool pAddLogNotice = false);

} // namespace governikus
