/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	id: contentBackground
	visible: false
	width: parent.width
	height: 0
	color: Constants.white

	default property alias columnChildren: contentColumn.children
	readonly property alias contentHeight: contentColumn.height

	Column {
		id: contentColumn

		width: parent.width
		anchors.top: parent.top
		topPadding: parent.width * 0.15
		bottomPadding: Constants.component_spacing
		spacing: Style.dimens.tutorial_component_spacing
	}
}
