/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Style
import Governikus.View

Button {
	id: control

	enum Type {
		Forward,
		Back,
		Check,
		Cancel
	}

	property string accessibleText
	property int buttonType: NavigationButton.Type.Forward
	property string iconSource
	property string iconText
	property double size: plugin.scaleFactor * 160
	property string subText

	Accessible.name: accessibleText !== "" ? accessibleText : text
	Accessible.role: Accessible.Button
	implicitHeight: column.implicitHeight
	implicitWidth: column.implicitWidth
	text: subText !== "" ? subText : (buttonType === NavigationButton.Type.Check ? qsTr("Yes") : buttonType === NavigationButton.Type.Cancel ? qsTr("No") : buttonType === NavigationButton.Type.Forward ? qsTr("Continue") : qsTr("Back"))

	background: Item {
	}
	contentItem: Item {
	}

	ColumnLayout {
		id: column

		anchors.fill: parent
		spacing: Constants.component_spacing

		Rectangle {
			id: icon

			Layout.alignment: Qt.AlignHCenter
			Layout.preferredHeight: Layout.preferredWidth
			Layout.preferredWidth: control.size
			color: enabled ? Style.color.control : Style.color.control_disabled
			radius: height / 2

			TintableIcon {
				anchors.centerIn: parent
				rotation: iconSource === "" && buttonType === NavigationButton.Type.Back ? 180 : 0
				source: {
					if (iconSource !== "") {
						return iconSource;
					}
					if (iconText !== "") {
						return "";
					}
					return buttonType === NavigationButton.Type.Check ? "qrc:///images/material_check.svg" : buttonType === NavigationButton.Type.Cancel ? "qrc:///images/material_clear.svg" : "qrc:///images/desktop/material_arrow_forward.svg";
				}
				sourceSize.height: Style.dimens.large_icon_size
				tintColor: Style.color.control_content
			}
			GText {
				anchors.centerIn: parent
				text: iconText
				textStyle: Style.text.headline
				visible: iconText !== ""
			}
		}
		GText {
			id: buttonText

			Layout.alignment: Qt.AlignHCenter
			horizontalAlignment: Text.AlignHCenter
			text: control.subText
			textStyle: Style.text.headline
			visible: control.subText !== ""
		}
	}
	FocusFrame {
	}
	MouseArea {
		anchors.fill: parent
		cursorShape: Qt.PointingHandCursor

		onPressed: mouse => {
			mouse.accepted = false;
		}
	}
}
