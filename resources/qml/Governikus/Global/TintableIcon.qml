/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Style 1.0

Item {
	id: root

	property bool desaturate: false
	property alias fillMode: image.fillMode
	property alias paintedHeight: image.paintedHeight
	property alias paintedWidth: image.paintedWidth
	property alias playAnimation: animation.enabled
	property alias source: image.source
	property alias sourceSize: image.sourceSize
	property color tintColor: Style.color.primary_text
	property bool tintEnabled: true
	property alias transformOrigin: image.transformOrigin

	implicitHeight: image.implicitHeight
	implicitWidth: image.implicitWidth

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
		Behavior on source  {
			id: animation
			enabled: false

			SequentialAnimation {
				PropertyAnimation {
					duration: Constants.animation_duration
					easing.type: Easing.InCubic
					property: "opacity"
					targets: image
					to: 0
				}
				PropertyAction {
					property: "source"
					target: image
				}
				PropertyAnimation {
					duration: Constants.animation_duration
					easing.type: Easing.OutCubic
					property: "opacity"
					targets: image
					to: 1
				}
			}
		}
	}
}
