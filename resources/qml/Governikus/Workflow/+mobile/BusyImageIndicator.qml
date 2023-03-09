/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0

Item {
	property string image
	property alias running: busyIndicator.running

	GBusyIndicator {
		id: busyIndicator
		anchors.centerIn: parent
		factor: 1.2
		height: img.height
		running: false
		width: height
	}
	Image {
		id: img
		anchors.centerIn: parent
		fillMode: Image.PreserveAspectFit
		smooth: true
		source: image
		width: parent.width
	}
}
