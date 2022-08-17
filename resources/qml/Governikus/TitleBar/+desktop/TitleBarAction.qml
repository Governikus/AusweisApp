/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0

FocusScope {
	id: scope

	height: row.height
	width: row.width

	signal clicked()

	property alias showArrow: arrow.visible
	property alias text: text.text

	property Item customSubAction: Item { visible: false }

	property bool rootEnabled: true
	property bool active: true

	property bool showSettings: false
	property var customSettingsHandler
	readonly property var settingsHandler: customSettingsHandler ? customSettingsHandler : function() {}

	property bool showHelp: true
	property string helpTopic: "applicationPage"

	property int list_index: 1
	property int list_length: 1

	readonly property bool isLastElement: list_index === list_length
	readonly property bool isClickable: active && !isLastElement

	Accessible.role: Accessible.Button
	Accessible.name: !scope.active
				//: LABEL DESKTOP
				? qsTr("Navigating to %1 in current context disabled").arg(text.text) + ", "
					//: LABEL DESKTOP
					+ qsTr("element %1 of %2").arg(list_index).arg(list_length)
				: isLastElement
					//: LABEL DESKTOP
					? qsTr("Current context: %1").arg(text.text) + ", "
						//: LABEL DESKTOP
						+ qsTr("element %1 of %2").arg(list_index).arg(list_length)
					//: LABEL DESKTOP
					: qsTr("Navigate to %1").arg(text.text) + ", "
						//: LABEL DESKTOP
						+ qsTr("element %1 of %2").arg(list_index).arg(list_length)
	activeFocusOnTab: true

	Keys.onSpacePressed: if (isClickable) scope.clicked()

	Row {
		id: row

		height: text.height
		spacing: Style.dimens.titlebar_padding

		TintableIcon {
			id: arrow

			sourceSize.width: Style.dimens.icon_size
			anchors.bottom: text.baseline
			anchors.bottomMargin: 1 * ApplicationModel.scaleFactor

			source: "qrc:///images/desktop/titlebar_arrow.svg"
			tintColor: Style.text.header_inverse.textColor
		}

		TitleBarText {
			id: text

			readonly property color textColor: scope.active ? Style.text.header_inverse.textColor : Style.text.header_secondary_inverse.textColor
			readonly property color pressColor: Qt.darker(textColor,  Constants.highlightDarkerFactor)

			Accessible.role: Accessible.Button
			Accessible.name: text.text

			color: mouseArea.containsPress ? pressColor : textColor
			font.bold: isLastElement

			FocusFrame {
				scope: scope
				isOnLightBackground: false
			}

			MouseArea {
				id: mouseArea

				anchors.fill: parent
				cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
				onPressed: scope.focus = true
				onClicked: scope.clicked()
				enabled: scope.isClickable
			}
		}
	}
}
