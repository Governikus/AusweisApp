/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Controls

import Governikus.Style
import Governikus.Type
import Governikus.View

TextField {
	id: root

	property int enterKeyType: Qt.EnterKeyDefault
	property var textStyle: Style.text.normal

	Accessible.editable: true
	Accessible.name: echoMode === TextInput.Normal ? displayText + d.delimiter + d.maxAllowedText : ""
	Accessible.role: Accessible.EditableText
	EnterKey.type: enterKeyType
	color: textStyle.textColor
	font.family: UiPluginModel.fontFamily
	font.pixelSize: textStyle.textSize
	inputMethodHints: Qt.ImhNoPredictiveText
	placeholderTextColor: Style.color.textNormal.basic_unchecked
	selectByMouse: true
	selectedTextColor: Style.color.textNormal.basic_unchecked
	selectionColor: Style.color.control.background.basic_unchecked

	background: Rectangle {
		border.color: Style.color.border
		border.width: Style.dimens.border_width
		color: Style.color.pane.background.basic_unchecked
		radius: Style.dimens.control_radius
	}

	Accessible.onScrollDownAction: Utils.scrollPageDownOnGFlickable(this)
	Accessible.onScrollUpAction: Utils.scrollPageUpOnGFlickable(this)

	FocusFrame {
		scope: root
	}
	QtObject {
		id: d

		readonly property string delimiter: root.displayText !== "" && root.maximumLength > 0 ? ". " : ""
		//: ALL_PLATFORMS
		readonly property string maxAllowedText: root.maximumLength > 0 ? qsTr("A maximum of %1 characters is allowed.").arg(root.maximumLength) : ""
	}
}
