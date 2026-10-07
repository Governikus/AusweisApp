/**
 * Copyright (c) 2019-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import Governikus.Style

Rectangle {
	property int orientation: Qt.Horizontal // Qt.Vertical

	color: Style.color.border
	implicitHeight: orientation === Qt.Horizontal ? Style.dimens.border_width : 0
	implicitWidth: orientation === Qt.Vertical ? Style.dimens.border_width : 0
}
