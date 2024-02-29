/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Style
import Governikus.View

TextField {
	id: baseItem

	property int enterKeyType: Qt.EnterKeyDefault
	property var textStyle: Style.text.normal

	Accessible.editable: true
	Accessible.name: echoMode === TextInput.Normal ? displayText : ""
	Accessible.role: Accessible.EditableText
	EnterKey.type: enterKeyType
	ToolTip.text: qsTr("Maximum allowed length reached.")
	ToolTip.visible: text.length === maximumLength && focus
	activeFocusOnTab: true
	color: textStyle.textColor
	font.pixelSize: textStyle.textSize
	placeholderTextColor: Style.color.text
	selectByMouse: true
	selectedTextColor: Style.color.text
	selectionColor: Style.color.control

	background: Rectangle {
		border.color: Style.color.border
		border.width: Style.dimens.separator_size
		color: Style.color.pane
		radius: Style.dimens.control_radius
	}

	FocusFrame {
		scope: baseItem
	}
}
