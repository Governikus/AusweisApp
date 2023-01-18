/*
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Style 1.0
import Governikus.View 1.0

TextField {
	id: baseItem

	property int enterKeyType: Qt.EnterKeyDefault
	property bool isOnLightBackground: true
	property var textStyle: Style.text.normal
	property bool valid: true

	Accessible.editable: true
	Accessible.name: echoMode === TextInput.Normal ? displayText : ""
	Accessible.role: Accessible.EditableText
	EnterKey.type: enterKeyType
	ToolTip.text: qsTr("Maximum allowed length reached.")
	ToolTip.visible: text.length === maximumLength && focus
	activeFocusOnTab: true
	color: textStyle.textColor
	font.pixelSize: textStyle.textSize
	placeholderTextColor: Style.color.secondary_text
	selectByMouse: true
	selectedTextColor: Style.color.primary_text_inverse
	selectionColor: Style.color.accent

	background: Rectangle {
		border.color: baseItem.valid ? Style.color.border : Constants.red
		border.width: Style.dimens.separator_size
		color: baseItem.enabled ? Style.color.background_pane : Constants.grey
		radius: Style.dimens.button_radius
	}

	FocusFrame {
		isOnLightBackground: baseItem.isOnLightBackground
		scope: baseItem
	}
}
