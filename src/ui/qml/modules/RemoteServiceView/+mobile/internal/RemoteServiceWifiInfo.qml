/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

RowLayout {
	spacing: Style.dimens.text_spacing

	TintableIcon {
		source: "qrc:/images/info.svg"
		sourceSize.width: Style.dimens.small_icon_size
		tintColor: infoText.color
	}
	GText {
		id: infoText

		color: Style.color.textSubline.basic

		//: INFO ANDROID IOS The remote service is active. Hint that both devices need to be connected to the same network.
		text: qsTr("Both devices have to be on the same network (e.g. WiFi).")
	}
	GSpacer {
		Layout.fillWidth: true
	}
}
