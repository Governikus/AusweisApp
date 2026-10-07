/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type
import Governikus.View

FlickableSectionPage {
	id: root

	spacing: Style.dimens.pane_spacing

	//: MOBILE
	title: qsTr("Device name")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}

	onActivate: if (!ApplicationModel.screenReaderRunning)
		textField.forceActiveFocus(Qt.MouseFocusReason)

	GOptionsContainer {
		containerPadding: Style.dimens.pane_padding
		containerSpacing: Style.dimens.groupbox_spacing
		//: MOBILE
		title: qsTr("Change device name")

		GTextField {
			id: textField

			enterKeyType: Qt.EnterKeyDone
			maximumLength: 33

			Component.onCompleted: text = SettingsModel.deviceName
			onAccepted: root.pop()
			onTextEdited: SettingsModel.deviceName = text
		}
	}
}
