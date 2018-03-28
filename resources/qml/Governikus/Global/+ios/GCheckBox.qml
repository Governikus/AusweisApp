import QtQuick 2.7

import "."

Item {
	property bool checked: false
	property alias text: description.text

	height: Utils.dp(20)
	width: row.width

	Row {
		id: row
		height: parent.height
		spacing: Utils.dp(6)

		Image {
			id: image
			source: checked && enabled ? "qrc:///images/iOS/CheckedCheckbox.png" : ""
			height: parent.height
			width: height
			fillMode: Image.PreserveAspectFit
		}

		Text {
			id: description
			color: Constants.secondary_text
			visible: text !== ""
			anchors.verticalCenter: image.verticalCenter
			font.pixelSize: Constants.normal_font_size
		}
	}

	MouseArea {
		anchors.fill: row
		onClicked: if (enabled) checked = !checked
	}
}
