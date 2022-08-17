/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Style 1.0

Item {
	id: root

	property alias source: image.source
	property alias sourceSize: image.sourceSize
	property alias transformOrigin: image.transformOrigin
	property alias fillMode: image.fillMode
	property alias playAnimation: animation.enabled
	property bool desaturate: false
	property bool tintEnabled: true
	property color tintColor: Style.color.primary_text

	implicitWidth: image.implicitWidth
	implicitHeight: image.implicitHeight

	Image {
		id: image

		anchors.fill: parent

		asynchronous: true
		fillMode: Image.PreserveAspectFit

		layer.enabled: root.tintEnabled && GraphicsInfo.api !== GraphicsInfo.Software
		layer.effect: ShaderEffect {
			property color color: root.tintColor

			fragmentShader: root.desaturate ? "qrc:/shader/DesaturateShader.frag" : "qrc:/shader/ColorOverlayShader.frag"
		}

		Behavior on source {
			id: animation

			enabled: false

			SequentialAnimation {
				PropertyAnimation {
					targets: image
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
					targets: image
					property: "opacity"
					to: 1
					duration: Constants.animation_duration
					easing.type: Easing.OutCubic
				}
			}
		}
	}
}
