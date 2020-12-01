/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Style 1.0

Item {
	property string image
	property alias running: busyIndicator.running

	BusyIndicator {
		id: busyIndicator
		anchors.centerIn: parent
		height: img.height
		width: height
		running: false
		contentItem: NpaBusyIndicatorStyle {factor: 1.2}
	}

	Image {
		id: img
		anchors.centerIn: parent
		width: parent.width
		fillMode: Image.PreserveAspectFit
		smooth: true
		source: image
	}
}
