import QtQuick 2.10
import QtQuick.Controls 2.3

import "Utils.js" as Utils

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
		border.color: Constants.red
		color: enabled ? "white" : Constants.grey
		border.width: valid ? 0 : Utils.dp(2)
	}

	TextField {
		id: field
		anchors.fill: parent
		anchors.leftMargin: Utils.dp(6)
		anchors.rightMargin: Utils.dp(6)
		padding: 0
		font.pixelSize: Constants.normal_font_size
		onAccepted: parent.accepted()
		background: Item {}
	}

	onActiveFocusChanged: if (focus) field.forceActiveFocus()
}
