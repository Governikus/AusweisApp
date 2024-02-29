/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Style

Image {
	id: root

	property bool desaturate: false
	property alias playAnimation: animation.enabled
	property color tintColor: Style.color.text
	property bool tintEnabled: true

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
				duration: source == "" ? 0 : Constants.animation_duration
				easing.type: Easing.InCubic
				property: "opacity"
				targets: root
				to: 0
			}
			PropertyAction {
				property: "source"
				target: root
			}
			PropertyAnimation {
				duration: source == "" ? Constants.animation_duration * 2 : Constants.animation_duration
				easing.type: source == "" ? Easing.InOutCubic : Easing.InCubic
				property: "opacity"
				targets: root
				to: 1
			}
		}
	}
}
