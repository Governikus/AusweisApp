import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import "Utils.js" as Utils
import "." as Gov

Item {
	property alias text: field.text
	property alias displayText: field.displayText
	property alias echoMode: field.echoMode
	signal accepted

	property bool valid: true

	height: Utils.dp(24)
	width: Utils.dp(240)

	Rectangle {
		radius: Utils.dp(6)
		anchors.fill: parent
		border.color: Gov.Constants.red
		color: enabled ? "white" : Gov.Constants.grey
		border.width: valid ? 0 : Utils.dp(2)
	}

	TextField {
		id: field
		anchors.fill: parent
		anchors.leftMargin: Utils.dp(6)
		anchors.rightMargin: Utils.dp(6)
		font.pixelSize: Gov.Constants.normal_font_size
		onAccepted: parent.accepted()
		style: TextFieldStyle {
			background: Rectangle {}
		}
	}

	onActiveFocusChanged: if (focus) field.forceActiveFocus()
}
