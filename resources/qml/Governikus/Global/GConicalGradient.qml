/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

Item {
	id: root

	property int verticalOffset: 0
	property int horizontalOffset: 0
	property real angle: 0

	property var source: Rectangle {
		width: root.width
		height: root.height
	}

	property Gradient gradient: Gradient {
		GradientStop { position: 0.0; color: "white" }
		GradientStop { position: 1.0; color: "black" }
	}

	ShaderEffectSource {
		id: baseSource

		anchors.fill: parent

		sourceItem: root.source
		smooth: true
		visible: false
	}

	ShaderEffect {
		id: colorEffect

		property var source: baseSource
		property real startAngle: (root.angle - 90) * Math.PI/180
		property var center: Qt.point(0.5 + root.horizontalOffset / width, 0.5 + root.verticalOffset / height)
		property var gradientSource: ShaderEffectSource {
			sourceItem: Rectangle {
				width: 16
				height: 256
				gradient: root.gradient
				smooth: true
			}

			smooth: true
			hideSource: true
		}

		anchors.fill: baseSource

		smooth: true
		fragmentShader: "qrc:/shader/ConicalGradientShader.frag"
	}
}
