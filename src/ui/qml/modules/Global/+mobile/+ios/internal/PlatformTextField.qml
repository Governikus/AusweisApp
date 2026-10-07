/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Native
import Governikus.Style
import Governikus.Type

NativeTextField {
	id: root

	property int enterKeyType: Qt.EnterKeyDefault // only used by QTextField

	property var textStyle: Style.text.normal

	Layout.fillWidth: true
	backgroundColor: Style.color.pane.background.basic_unchecked
	borderColor: Style.color.border
	borderRadius: Style.dimens.control_radius
	borderWidth: Style.dimens.border_width
	font.family: UiPluginModel.fontFamily
	font.pixelSize: textStyle.textSize
	textColor: textStyle.textColor

	WindowContainer {
		Accessible.ignored: false
		anchors.fill: parent
		window: root.nativeWindow
	}
}
