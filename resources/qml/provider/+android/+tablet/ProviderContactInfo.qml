import QtQuick 2.6
import QtQuick.Layouts 1.2

import "../global"


Rectangle {
	id: baseItem
	property alias contactModel: contactListView.model

	onHeightChanged: { info.scalingAllowed = true; info.updateScaleFactor() }
	onVisibleChanged: info.scalingAllowed = visible

	Column {
		id: info
		width: baseItem.width / info.scaleFactor // fill whole width
		transform: Scale { yScale: info.scaleFactor; xScale: info.scaleFactor }
		property bool scalingAllowed: true
		property real scaleFactor: 1

		function updateScaleFactor() {
			if (scalingAllowed) {
				scalingAllowed = false; // just scale once to prevent flickering of height and a corresponding deadlock
				scaleFactor =  height > 0 && baseItem.height > 0 ? baseItem.height/height : 1
			}
		}
		onHeightChanged: info.updateScaleFactor()

		Text {
			text: qsTr("Contact")
			padding: Constants.component_spacing
			font.pixelSize: Constants.header_font_size
			color: "white"
		}
		Rectangle {
			anchors.left: parent.left
			anchors.right: parent.right
			height: contactListView.height
			ListView {
				id: contactListView
				width: parent.width
				height: contentHeight
				interactive: false
				spacing: 2
				delegate:  ProviderContactInfoItem {
					width: contactListView.width
					color: baseItem.color
					imageSource: Qt.resolvedUrl(model.iconSource)
					itemText: !!model.text ? model.text : qsTr("Unknown")
					link: model.link
				}
			}
		}
	}
}
