/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
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
	onActiveFocusChanged: if (activeFocus)
		Utils.positionViewAtItem(this, this.parent, Qt.platform.os === "android")

	FocusFrame {
		scope: root
	}
}
