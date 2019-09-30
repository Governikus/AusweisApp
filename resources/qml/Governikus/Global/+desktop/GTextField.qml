/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0


Item {
	id: baseItem

	property alias text: field.text
	property var textStyle: Style.text.normal
	property alias displayText: field.displayText
	property alias echoMode: field.echoMode
	property alias textAnchors: field.anchors
	property alias validator: field.validator
	property alias maximumLength: field.maximumLength
	readonly property alias acceptableInput: field.acceptableInput
	signal accepted

	property bool valid: true

	height: field.height + 2 * textBackground.radius
	width: ApplicationModel.scaleFactor * 240

	Rectangle {
		id: textBackground

		anchors.fill: parent

		radius: ApplicationModel.scaleFactor * 6
		color: enabled ? Constants.white : Constants.grey
		border.color: Constants.red
		border.width: valid ? 0 : ApplicationModel.scaleFactor * 2
	}

	TextField {
		id: field

		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: textBackground.radius

		activeFocusOnTab: true
		padding: 0
		font.pixelSize: baseItem.textStyle.textSize
		color: baseItem.textStyle.textColor
		onAccepted: baseItem.accepted()
		background: Item {}
	}

	onActiveFocusChanged: if (focus) field.forceActiveFocus()

	FocusFrame {
		scope: field
	}
}
