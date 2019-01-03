import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0

Row {
	id: logControls

	signal share()
	signal remove()
	signal removeAll()

	readonly property int contentWidth: width
	property alias allowRemove: removeButton.visible

	spacing: Utils.dp(18)
	anchors.verticalCenter: parent ? parent.verticalCenter : undefined

	Image {
		height: Constants.titlebar_font_size * 1.5
		width: height
		fillMode: Image.PreserveAspectFit
		source: "qrc:///images/share.svg"

		MouseArea {
			anchors.fill: parent
			anchors.margins: -Utils.dp(8)
			onClicked: logControls.share()
		}
	}

	Image {
		id: removeButton
		height: Constants.titlebar_font_size * 1.5
		width: height
		fillMode: Image.PreserveAspectFit
		source: "qrc:///images/trash_icon_white.svg"

		MouseArea {
			anchors.fill: parent
			anchors.margins: -Utils.dp(8)
			onClicked: logControls.remove()
		}
	}

	Image {
		height: Constants.titlebar_font_size * 1.5
		width: height
		fillMode: Image.PreserveAspectFit
		source: "qrc:///images/trash_icon_all.svg"

		MouseArea {
			anchors.fill: parent
			anchors.margins: -Utils.dp(8)
			onClicked: logControls.removeAll()
		}
	}
 }
