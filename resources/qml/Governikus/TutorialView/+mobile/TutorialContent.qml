/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	id: contentBackground

	default property alias columnChildren: contentColumn.children
	readonly property alias contentHeight: contentColumn.height

	color: Constants.white
	height: 0
	visible: false
	width: parent.width

	Column {
		id: contentColumn
		anchors.top: parent.top
		bottomPadding: Constants.component_spacing
		spacing: Style.dimens.tutorial_component_spacing
		topPadding: parent.width * 0.15
		width: parent.width
	}
}
