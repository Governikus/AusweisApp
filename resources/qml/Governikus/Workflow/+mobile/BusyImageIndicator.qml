/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0

Item {
	property string image
	property alias running: busyIndicator.running

	GBusyIndicator {
		id: busyIndicator

		width: height
		height: img.height
		anchors.centerIn: parent

		running: false
		factor: 1.2
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
