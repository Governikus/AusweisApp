/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


FocusScope {
	id: root

	property alias source: img.source
	property alias text: label.text
	property int iconSize: ApplicationModel.scaleFactor * 120

	signal clicked()

	implicitHeight: label.height + label.anchors.topMargin + img.height
	implicitWidth: Math.max(label.width, img.width)

	Accessible.role: Accessible.Button
	Accessible.name: text

	Keys.onSpacePressed: root.clicked()

	MouseArea {
		anchors.fill: focusFrame
		cursorShape: Qt.PointingHandCursor
		onPressed: parent.focus = true
		onClicked: parent.clicked()
	}

	FocusFrame {
		id: focusFrame
		framee: img
		marginFactor: 2.0
	}

	FramedImage {
		id: img
		width: iconSize
		height: iconSize
		anchors.top: parent.top
		border.color: Constants.white
		anchors.horizontalCenter: parent.horizontalCenter
	}

	GText {
		id: label
		width: parent.width
		anchors.top: img.bottom
		anchors.topMargin: Constants.component_spacing
		horizontalAlignment: Text.AlignHCenter
		font.bold: true
	}
}
