import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.View 1.0

FocusScope {
	id: scope

	height: row.height
	width: row.width

	signal clicked()

	property alias showArrow: arrow.visible
	property alias text: text.text

	property Item customSubAction: Item { visible: false }

	property bool showSettings: true
	property var customSettingsHandler
	readonly property var settingsHandler: customSettingsHandler ? customSettingsHandler : function() {
		// TODO open settings
	}

	property bool showHelp: true
	property string helpTopic: "applicationPage"

	Accessible.role: Accessible.Button
	Accessible.name: text.text

	Keys.onSpacePressed: scope.clicked()

	Row {
		id: row

		height: text.height
		spacing: Constants.titlebar_spacing

		Image {
			id: arrow
			sourceSize.height: text.height / 2.5
			anchors.verticalCenter: text.verticalCenter
			anchors.verticalCenterOffset: height / 6
			source: "qrc:///images/desktop/titlebar_arrow.svg"
		}

		TitleBarText {
			id: text

			Accessible.role: Accessible.Button
			Accessible.name: text.text

			FocusFrame {
				scope: scope
			}

			MouseArea {
				anchors.fill: parent
				cursorShape: Qt.PointingHandCursor
				onPressed: scope.focus = true
				onClicked: scope.clicked()
			}
		}
	}
}
