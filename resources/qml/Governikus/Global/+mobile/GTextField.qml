/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Style 1.0

import "Utils.js" as Utils

Item {
	property alias text: field.text
	property alias displayText: field.displayText
	property alias echoMode: field.echoMode
	property int enterKeyType: Qt.EnterKeyDefault
	signal accepted

	property bool valid: true

	height: 24
	width: 240

	Accessible.role: Accessible.EditableText
	Accessible.editable: true
	Accessible.name: echoMode === TextInput.Normal ? displayText : ""

	Rectangle {
		radius: 6
		anchors.fill: parent
		border.color: Constants.red
		color: enabled ? Constants.white : Constants.grey
		border.width: valid ? 0 : 2
	}

	TextField {
		id: field
		anchors.fill: parent
		anchors.leftMargin: 6
		anchors.rightMargin: 6
		padding: 0
		font.pixelSize: Style.dimens.normal_font_size
		onAccepted: parent.accepted()
		background: Item {}
		EnterKey.type: enterKeyType
	}

	onActiveFocusChanged: if (focus) field.forceActiveFocus()
}
