import QtQuick 2.7

import "."

Item {
	property alias checked: box.checked
	property alias text: description.text

	height: Utils.dp(20)
	width: row.width

	Row {
		id: row
		height: parent.height
		spacing: Utils.dp(6)

		Rectangle {
			id: box
			property bool checked

			height: parent.height
			width: height

			color: enabled ? (checked ? Constants.accent_color : "white") : Constants.grey
			border.color: checked ? Constants.accent_color : "black"
			border.width: Utils.dp(2)
			radius: Utils.dp(2)

			Image {
				source: "qrc:///images/check.svg"
				anchors.fill: parent
				anchors.margins: Utils.dp(3)
				fillMode: Image.PreserveAspectFit
				visible: checked && enabled
			}
		}

		Text {
			id: description
			visible: text !== ""
			anchors.verticalCenter: box.verticalCenter
			font.pixelSize: Constants.normal_font_size
		}
	}

	MouseArea {
		anchors.fill: row
		onClicked: if (enabled) box.checked = !box.checked
	}
}
