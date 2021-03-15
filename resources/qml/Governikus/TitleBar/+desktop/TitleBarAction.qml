/*
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

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

	property bool showSettings: false
	property var customSettingsHandler
	readonly property var settingsHandler: customSettingsHandler ? customSettingsHandler : function() {}

	property bool showHelp: true
	property string helpTopic: "applicationPage"

	Accessible.role: Accessible.Button
	Accessible.name: text.text
	activeFocusOnTab: true

	Keys.onSpacePressed: scope.clicked()

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

			readonly property color textColor: scope.enabled ? Style.text.header_inverse.textColor : Style.text.header_secondary_inverse.textColor
			readonly property color pressColor: Qt.darker(textColor,  Constants.highlightDarkerFactor)

			Accessible.role: Accessible.Button
			Accessible.name: text.text

			color: mouseArea.containsPress ? pressColor : textColor

			FocusFrame {
				scope: scope
			}

			MouseArea {
				id: mouseArea

				anchors.fill: parent
				cursorShape: Qt.PointingHandCursor
				onPressed: scope.focus = true
				onClicked: scope.clicked()
			}
		}
	}
}
