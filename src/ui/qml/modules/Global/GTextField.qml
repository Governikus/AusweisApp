/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Style
import Governikus.View

TextField {
	id: root

	property int enterKeyType: Qt.EnterKeyDefault
	property var textStyle: Style.text.normal

	Accessible.editable: true
	Accessible.name: echoMode === TextInput.Normal ? displayText : ""
	Accessible.role: Accessible.EditableText
	EnterKey.type: enterKeyType
	ToolTip.text: qsTr("Maximum allowed length reached.")
	ToolTip.visible: text.length === maximumLength && focus
	color: textStyle.textColor
	font.pixelSize: textStyle.textSize
	placeholderTextColor: Style.color.textNormal.basic
	selectByMouse: true
	selectedTextColor: Style.color.textNormal.basic
	selectionColor: Style.color.control.background.basic

	background: Rectangle {
		border.color: Style.color.border
		border.width: Style.dimens.separator_size
		color: Style.color.pane.background.basic
		radius: Style.dimens.control_radius
	}

	FocusFrame {
		scope: root
	}
}
