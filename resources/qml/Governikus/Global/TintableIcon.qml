/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Style 1.0

Item {
	property alias source: image.source
	property alias sourceSize: image.sourceSize
	property alias transformOrigin: image.transformOrigin
	property alias fillMode: image.fillMode
	property bool tintEnabled: true
	property color tintColor: Style.color.primary_text

	implicitWidth: image.implicitWidth
	implicitHeight: image.implicitHeight

	Image {
		id: image

		visible: !tintEnabled || GraphicsInfo.api === GraphicsInfo.Software
		anchors.fill: parent

		fillMode: Image.PreserveAspectFit
	}

	ColorOverlay {
		visible: tintEnabled && GraphicsInfo.api !== GraphicsInfo.Software
		anchors.fill: parent

		cached: true
		color: tintColor
		source: image
	}
}
