import QtQuick 2.5

import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import "../global"


Row {
	id: root

	property int availableWidth: 0
	readonly property int contentWidth: root.implicitWidth
	readonly property alias searchText: searchField.text

	anchors.top: parent ? parent.top : undefined
	anchors.topMargin: Utils.dp(10)
	anchors.bottom: parent ? parent.bottom : undefined
	anchors.bottomMargin: anchors.topMargin
	spacing: Utils.dp(24)

	TextField {
		id: searchField
		height: parent.height
		width: root.availableWidth - iconItem.width - 2 * parent.spacing
		horizontalAlignment: Text.AlignLeft
		visible: false
		property string savedText: ""

		Behavior on visible {
			PropertyAnimation {
				duration: 150
			}
		}

		onVisibleChanged: {
			// When search bar becomes visible again, restore saved search text.
			if (visible) {
				text = savedText
			}
			else {
				savedText = text
				text = ""
			}
		}

		style: TextFieldStyle {
			background: Rectangle {
				radius: Utils.dp(6)
			}
		}
	}

	Image {
		id: iconItem

		height: parent.height
		width: height
		fillMode: Image.PreserveAspectFit
		source: "qrc:///images/magnifying-glass.png"

		MouseArea {
			anchors.fill: parent
			onClicked: {
				searchField.visible = !searchField.visible
			}
		}
	}
}
