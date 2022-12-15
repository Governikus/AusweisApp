/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

TintableIcon {
	id: root
	signal clicked

	Accessible.role: Accessible.Button
	anchors.verticalCenter: parent ? parent.verticalCenter : undefined
	height: Style.dimens.small_icon_size
	sourceSize.height: height
	tintColor: Style.color.button_text
	width: height

	Accessible.onPressAction: mouseArea.clicked(null)

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		anchors.margins: -8

		onClicked: root.clicked()
	}
}
