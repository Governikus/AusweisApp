/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ApplicationModel 1.0


Button {
	id: control

	enum Type {
		Forward,
		Back,
		Check,
		Cancel
	}

	property int buttonType
	property string subText

	width: column.width
	height: column.height

	Accessible.role: Accessible.Button
	text: subText !== "" ? subText : ((buttonType === NavigationButton.Type.Check   ? qsTr("Yes") :
									   buttonType === NavigationButton.Type.Cancel  ? qsTr("No") :
									   buttonType === NavigationButton.Type.Forward ? qsTr("Continue") :
									   qsTr("Back")) + SettingsModel.translationTrigger)

	background: Item {}
	contentItem: Item {}

	Column {
		id: column

		spacing: Constants.component_spacing

		Rectangle {
			id: icon

			width: ApplicationModel.scaleFactor * 160
			height: width
			anchors.horizontalCenter: parent.horizontalCenter

			radius: height / 2
			border.width: height / 40;
			border.color: enabled ? Style.color.primary_text_inverse : Constants.red
			color: Style.color.button

			TintableIcon {
				anchors.centerIn: parent

				source: buttonType == NavigationButton.Type.Check  ? "qrc:///images/check.svg" :
						buttonType == NavigationButton.Type.Cancel ? "qrc:///images/cancel.svg" :
																	 "qrc:///images/desktop/continue_arrow.svg"
				sourceSize.height: Style.dimens.large_icon_size
				transformOrigin: Item.Center
				rotation: buttonType == NavigationButton.Type.Back ? 180 : 0
				tintColor: Style.color.button_text
			}
		}

		GText {
			id: buttonText

			visible: control.subText !== ""
			anchors.horizontalCenter: parent.horizontalCenter

			text: control.subText
			textStyle: Style.text.header_inverse
		}
	}

	FocusFrame {}

	MouseArea {
		anchors.fill: parent

		onPressed: mouse.accepted = false
		cursorShape: Qt.PointingHandCursor
	}
}
