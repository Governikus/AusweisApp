/**
 * Copyright (c) 2019-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "GlobalStatus.h"

#include <QString>
#include <QUrl>


namespace governikus
{

class GlobalStatus;

QString generateMailHeader(const GlobalStatus& pStatus, bool pPercentEncoding = false);
QString generateMailBody(const GlobalStatus& pStatus, const QUrl& pServiceUrl = {}, bool pPercentEncoding = false, bool pAddLogNotice = false);

} // namespace governikus
