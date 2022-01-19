/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtGraphicalEffects 1.12

import Governikus.Style 1.0

Item {
	property alias source: image.source
	property alias sourceSize: image.sourceSize
	property alias transformOrigin: image.transformOrigin
	property alias fillMode: image.fillMode
	property alias playAnimation: animation.enabled
	property bool tintEnabled: true
	property color tintColor: Style.color.primary_text

	implicitWidth: image.implicitWidth
	implicitHeight: image.implicitHeight

	Image {
		id: image

		visible: !tintEnabled || GraphicsInfo.api === GraphicsInfo.Software
		anchors.fill: parent

		fillMode: Image.PreserveAspectFit

		Behavior on source {
			id: animation

			enabled: false

			SequentialAnimation {
				PropertyAnimation {
					targets: [image, overlay]
					property: "opacity"
					to: 0
					duration: Constants.animation_duration
					easing.type: Easing.InCubic
				}
				PropertyAction {
					target: image
					property: "source"
				}
				PropertyAnimation {
					targets: [image, overlay]
					property: "opacity"
					to: 1
					duration: Constants.animation_duration
					easing.type: Easing.OutCubic
				}
			}
		}
	}

	ColorOverlay {
		id: overlay

		visible: tintEnabled && GraphicsInfo.api !== GraphicsInfo.Software
		anchors.fill: parent

		cached: true
		color: tintColor
		source: image
	}
}
