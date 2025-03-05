/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Style

Rectangle {
	property int orientation: Qt.Horizontal // Qt.Vertical

	color: Style.color.border
	implicitHeight: orientation === Qt.Horizontal ? Style.dimens.separator_size : 0
	implicitWidth: orientation === Qt.Vertical ? Style.dimens.separator_size : 0
}
