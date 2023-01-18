/*!
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>
#include <QUrl>

namespace governikus
{

class GlobalStatus;

QString generateMailBody(const GlobalStatus& pStatus, const QUrl& pServiceUrl = {}, bool pPercentEncoding = false, bool pAddLogNotice = false);

} // namespace governikus
