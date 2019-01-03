import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Type.ApplicationModel 1.0


Item {
	property alias text: field.text
	property alias font: field.font
	property alias displayText: field.displayText
	property alias echoMode: field.echoMode
	signal accepted

	property bool valid: true

	height: field.height + 2 * textBackground.radius
	width: ApplicationModel.scaleFactor * 240

	Rectangle {
		id: textBackground
		radius: ApplicationModel.scaleFactor * 6
		anchors.fill: parent
		border.color: Constants.red
		color: enabled ? "white" : Constants.grey
		border.width: valid ? 0 : ApplicationModel.scaleFactor * 2
	}

	TextField {
		id: field
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: textBackground.radius
		padding: 0
		font.pixelSize: Constants.normal_font_size
		onAccepted: parent.accepted()
		background: Item {}
	}

	onActiveFocusChanged: if (focus) field.forceActiveFocus()
}
