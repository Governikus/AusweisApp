import QtQuick 2.7

import "Utils.js" as Utils
import "." as Gov

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

			color: enabled ? (checked ? Gov.Constants.accent_color : "white") : Gov.Constants.grey
			border.color: checked ? Gov.Constants.accent_color : "black"
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
			color: Gov.Constants.secondary_text
			visible: text !== ""
			anchors.verticalCenter: box.verticalCenter
			font.pixelSize: Gov.Constants.normal_font_size
		}
	}

	MouseArea {
		anchors.fill: row
		onClicked: if (enabled) box.checked = !box.checked
	}
}
