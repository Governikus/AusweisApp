/*!
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>

namespace governikus
{

class GlobalStatus;

QString generateMailBody(const GlobalStatus&);

} // namespace governikus
