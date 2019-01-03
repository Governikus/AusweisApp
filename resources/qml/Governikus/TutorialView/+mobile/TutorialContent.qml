import QtQuick 2.10

import Governikus.Global 1.0

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
		spacing: Constants.tutorial_component_spacing
	}
}
