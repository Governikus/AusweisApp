/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.Type.NumberModel
import Governikus.View

Rectangle {
	//: LABEL DESKTOP
	Accessible.name: qsTr("Remaining ID card PIN attempts: %1").arg(iconText.text)
	Accessible.role: Accessible.StaticText
	activeFocusOnTab: true
	border.color: Style.color.control_border
	border.width: height / 40
	color: Style.color.transparent
	implicitHeight: Style.dimens.status_icon_small
	implicitWidth: implicitHeight
	radius: height / 2
	width: height

	Rectangle {
		id: content

		anchors.fill: parent
		anchors.margins: parent.height / 8
		border.color: Style.color.pane_border
		border.width: Style.dimens.border_width
		color: Style.color.control
		radius: height / 2

		GText {
			id: iconText

			Accessible.ignored: true
			anchors.centerIn: parent
			color: Style.color.control_content
			text: NumberModel.retryCounter
			textStyle: Style.text.headline
			visible: text !== ""
		}
	}
	FocusFrame {
	}
}
