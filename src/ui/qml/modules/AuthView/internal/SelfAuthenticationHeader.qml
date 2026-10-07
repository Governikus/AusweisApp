/**
 * Copyright (c) 2025-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style

RowLayout {
	property alias iconSize: icon.sourceSize.height

	spacing: Style.dimens.pane_spacing

	StatusAnimation {
		id: icon

		Layout.alignment: Qt.AlignTop
		symbol.type: Symbol.Type.CHECK
	}
	GText {
		Accessible.id: "SelfAuthenticationData_successText"
		//: ALL_PLATFORMS Status message that the self authentication successfully completed (1/2).
		text: qsTr("Successfully read data.") + "<br>" +
		//: ALL_PLATFORMS Status message that the self authentication successfully completed (2/2).
		qsTr("You may now remove your ID card from the device.")
	}
}
