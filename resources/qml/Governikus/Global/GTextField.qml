/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Style 1.0
import Governikus.View 1.0

TextField {
	id: baseItem

	property var textStyle: Style.text.normal
	property int enterKeyType: Qt.EnterKeyDefault
	property bool valid: true
	property bool isOnLightBackground: true

	Accessible.role: Accessible.EditableText
	Accessible.editable: true
	Accessible.name: echoMode === TextInput.Normal ? displayText : ""

	EnterKey.type: enterKeyType

	activeFocusOnTab: true
	font.pixelSize: textStyle.textSize
	color: textStyle.textColor
	selectByMouse: true
	selectedTextColor: Style.color.primary_text_inverse
	selectionColor: Style.color.accent
	placeholderTextColor: Style.color.secondary_text
	ToolTip.visible: text.length === maximumLength && focus
	ToolTip.text: qsTr("Maximum allowed length reached.")

	background: Rectangle {
		radius: Style.dimens.button_radius
		color: baseItem.enabled ? Style.color.background_pane : Constants.grey
		border.color: baseItem.valid ? Style.color.border : Constants.red
		border.width: Style.dimens.separator_size
	}

	FocusFrame {
		scope: baseItem
		isOnLightBackground: baseItem.isOnLightBackground
	}
}
