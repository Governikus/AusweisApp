/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Style 1.0

Rectangle {
	property int orientation: Qt.Horizontal // Qt.Vertical

	height: orientation === Qt.Horizontal ? Style.dimens.separator_size : undefined
	width: orientation === Qt.Vertical ? Style.dimens.separator_size : undefined

	color: Style.color.border
}
