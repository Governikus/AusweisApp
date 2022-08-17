/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0


Button {
	id: control

	enum Type {
		Forward,
		Back,
		Check,
		Cancel
	}

	property int buttonType: NavigationButton.Type.Forward
	property string subText
	property string iconSource
	property string iconText

	implicitWidth: column.implicitWidth
	implicitHeight: column.implicitHeight

	Accessible.role: Accessible.Button
	text: subText !== "" ? subText : (buttonType === NavigationButton.Type.Check   ? qsTr("Yes") :
									   buttonType === NavigationButton.Type.Cancel  ? qsTr("No") :
									   buttonType === NavigationButton.Type.Forward ? qsTr("Continue") :
									   qsTr("Back"))

	background: Item {}
	contentItem: Item {}

	ColumnLayout {
		id: column

		anchors.fill: parent

		spacing: Constants.component_spacing

		Rectangle {
			id: icon

			Layout.preferredWidth: ApplicationModel.scaleFactor * 160
			Layout.preferredHeight: Layout.preferredWidth
			Layout.alignment: Qt.AlignHCenter

			radius: height / 2
			color: enabled ? Style.color.button : Style.color.button_disabled

			TintableIcon {
				anchors.centerIn: parent

				source: {
					if (iconSource !== "") {
						return iconSource
					}

					if (iconText !== "") {
						return ""
					}

					return buttonType === NavigationButton.Type.Check  ? "qrc:///images/material_check.svg" :
						   buttonType === NavigationButton.Type.Cancel ? "qrc:///images/material_clear.svg" :
						   "qrc:///images/desktop/material_arrow_forward.svg"
				}
				sourceSize.height: Style.dimens.large_icon_size
				transformOrigin: Item.Center
				rotation: iconSource === "" && buttonType === NavigationButton.Type.Back ? 180 : 0
				tintColor: Style.color.button_text
			}

			GText {
				visible: iconText !== ""

				anchors.centerIn: parent

				text: iconText
				textStyle: Style.text.title_inverse
			}
		}

		GText {
			id: buttonText

			visible: control.subText !== ""
			Layout.fillWidth: true

			text: control.subText
			textStyle: Style.text.header
			horizontalAlignment: Text.AlignHCenter
		}
	}

	FocusFrame {}

	MouseArea {
		anchors.fill: parent

		onPressed: mouse => { mouse.accepted = false }
		cursorShape: Qt.PointingHandCursor
	}
}
