/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Style 1.0


Rectangle {
	property alias busy: busyIndicator.visible
	property alias source: image.source
	property alias text: text.text

	width: height
	radius: height / 2
	border.width: height / 40;
	border.color: Constants.white
	color: Style.color.transparent

	BusyIndicator {
		id: busyIndicator
		anchors.fill: parent
		visible: false
		running: parent.visible
		contentItem: NpaBusyIndicatorStyle { factor: 1 }
	}

	Rectangle {
		radius: height / 2
		anchors.fill: parent
		anchors.margins: parent.height / 8;
		color: Constants.white

		Image {
			id: image
			anchors.fill: parent
			anchors.margins: parent.height / 8
			sourceSize.height: Constants.is_desktop ? height : undefined
			fillMode: Image.PreserveAspectFit
			visible: source.toString().length > 0
		}

		GText {
			id: text
			anchors.centerIn: parent
			textStyle: Style.text.title_accent
			visible: text !== ""
		}
	}
}
