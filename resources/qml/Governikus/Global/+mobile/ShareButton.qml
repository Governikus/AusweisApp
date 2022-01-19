/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0

TintableIcon {
	id: root

	signal clicked()

	anchors.verticalCenter: parent ? parent.verticalCenter : undefined
	height: Style.dimens.small_icon_size
	width: height

	Accessible.role: Accessible.Button
	Accessible.name: qsTr("Share content")
	Accessible.onPressAction: if (Qt.platform.os === "ios") mouseArea.clicked(null)

	tintColor: Style.color.button_text
	sourceSize.height: height
	source: "qrc:///images/mobile/platform_specific_share.svg"

	MouseArea {
		id: mouseArea

		anchors.fill: parent
		anchors.margins: -8

		onClicked: root.clicked()
	}
}
