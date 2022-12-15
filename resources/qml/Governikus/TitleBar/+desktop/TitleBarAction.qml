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

	property bool active: true
	property var customSettingsHandler
	property Item customSubAction: Item {
		visible: false
	}
	property string helpTopic: "applicationPage"
	readonly property bool isClickable: active && !isLastElement
	readonly property bool isLastElement: list_index === list_length
	property int list_index: 1
	property int list_length: 1
	property bool rootEnabled: true
	readonly property var settingsHandler: customSettingsHandler ? customSettingsHandler : function () {}
	property alias showArrow: arrow.visible
	property bool showHelp: true
	property bool showSettings: false
	property alias text: text.text

	signal clicked

	Accessible.name: !scope.active ?
	//: LABEL DESKTOP
	qsTr("Navigating to %1 in current context disabled").arg(text.text) + ", " +
	//: LABEL DESKTOP
	qsTr("element %1 of %2").arg(list_index).arg(list_length) : isLastElement ?
	//: LABEL DESKTOP
	qsTr("Current context: %1").arg(text.text) + ", " +
	//: LABEL DESKTOP
	qsTr("element %1 of %2").arg(list_index).arg(list_length) :
	//: LABEL DESKTOP
	qsTr("Navigate to %1").arg(text.text) + ", " +
	//: LABEL DESKTOP
	qsTr("element %1 of %2").arg(list_index).arg(list_length)
	Accessible.role: Accessible.Button
	activeFocusOnTab: true
	height: row.height
	width: row.width

	Keys.onSpacePressed: if (isClickable)
		scope.clicked()

	Row {
		id: row
		height: text.height
		spacing: Style.dimens.titlebar_padding

		TintableIcon {
			id: arrow
			anchors.bottom: text.baseline
			anchors.bottomMargin: 1 * ApplicationModel.scaleFactor
			source: "qrc:///images/desktop/titlebar_arrow.svg"
			sourceSize.width: Style.dimens.icon_size
			tintColor: Style.text.header_inverse.textColor
		}
		TitleBarText {
			id: text

			readonly property color pressColor: Qt.darker(textColor, Constants.highlightDarkerFactor)
			readonly property color textColor: scope.active ? Style.text.header_inverse.textColor : Style.text.header_secondary_inverse.textColor

			Accessible.name: text.text
			Accessible.role: Accessible.Button
			color: mouseArea.containsPress ? pressColor : textColor
			font.bold: isLastElement

			FocusFrame {
				isOnLightBackground: false
				scope: scope
			}
			MouseArea {
				id: mouseArea
				anchors.fill: parent
				cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
				enabled: scope.isClickable

				onClicked: scope.clicked()
				onPressed: scope.focus = true
			}
		}
	}
}
