/*
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtGraphicalEffects 1.12

import Governikus.Style 1.0

DropShadow {
	radius: 6
	samples: 12
	color: Style.color.shadow
	verticalOffset: 2
}
