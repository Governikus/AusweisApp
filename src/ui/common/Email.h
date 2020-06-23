/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>
#include <QUrl>

namespace governikus
{

class GlobalStatus;

QString generateMailBody(const GlobalStatus& pStatus, const QUrl& pServiceUrl = {});

} // namespace governikus
