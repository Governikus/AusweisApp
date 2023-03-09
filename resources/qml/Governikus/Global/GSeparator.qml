/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Style 1.0

Rectangle {
	property int orientation: Qt.Horizontal // Qt.Vertical

	color: Style.color.border
	height: orientation === Qt.Horizontal ? Style.dimens.separator_size : undefined
	width: orientation === Qt.Vertical ? Style.dimens.separator_size : undefined
}
