/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import QtGraphicalEffects 1.12

Item {
	id: baseItem

	property alias color: colorOverlay.color
	property alias shaderSource: effectSource.sourceItem

	height: plugin.safeAreaMargins.bottom + Math.max(backToMenu.height, quitTutorial.height)

	property alias backRotation: leftArrow.rotation
	property alias backText: menuText.text
	property bool backToMenuActive: true

	signal menuClicked()
	signal quitTutorialClicked()

	state: "showOnlyQuit"
	states: [
		State { name: "showBothOptions"; when: baseItem.backToMenuActive
			PropertyChanges { target: backToMenu; opacity: 1 }
			AnchorChanges { target: quitTutorial; anchors.right: baseItem.right}
			AnchorChanges { target: quitTutorial; anchors.horizontalCenter: undefined}
		},
		State { name: "showOnlyQuit"; when: !baseItem.backToMenuActive
			AnchorChanges { target: quitTutorial; anchors.horizontalCenter: baseItem.horizontalCenter}
			AnchorChanges { target: quitTutorial; anchors.right: undefined}
			PropertyChanges { target: backToMenu; opacity: 0 }
		}
	]
	transitions: [
		Transition {
			PropertyAnimation { target: backToMenu; property: "opacity"}
			AnchorAnimation { duration: 500; easing.type: Easing.InOutQuad }
		}
	]
	onVisibleChanged: if (visible) {
		// When the user quits the tutorial on iOS while we're in the "showBothOptions" state, the
		// animation will finish when the MoreView is already active. This results in quitTutorial
		// being wrongly positioned the next time the users enters the tutorial. The following lines
		// work around this issue:
		if (state === "showOnlyQuit") {
			quitTutorial.anchors.horizontalCenter = baseItem.horizontalCenter
		}
	}

	ShaderEffectSource {
		id: effectSource

		anchors.fill: parent
		sourceRect: Qt.rect(baseItem.x, baseItem.y, baseItem.width, baseItem.height)
	}
	FastBlur {
		anchors.fill: effectSource
		source: effectSource
		radius: 32
	}
	ColorOverlay {
		id: colorOverlay

		anchors.fill: parent
		color: footer.color
		Behavior on color { ColorAnimation { duration: Constants.animation_duration } }
		opacity: 0.7
	}

	Item {
		id: backToMenu
		anchors.left: parent.left
		anchors.top: parent.top
		height: menuRow.height + 2 * Constants.component_spacing
		width: menuRow.width

		Accessible.name: menuText.text
		Accessible.onPressAction: if (Qt.platform.os === "ios") baseItem.menuClicked()

		MouseArea {
			anchors.fill: parent
			preventStealing: true
			onClicked: baseItem.menuClicked()
		}

		Row {
			id: menuRow
			height: menuText.height
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			padding: Constants.component_spacing
			spacing: Constants.component_spacing

			Image {
				id: leftArrow
				source: "qrc:///images/tutorial/arrows.svg"
				rotation: -90
				anchors.verticalCenter: parent.verticalCenter

				height: parent.height
				width: height * (sourceSize.width / sourceSize.height)
				fillMode: Image.PreserveAspectFit
			}

			GText {
				id: menuText

				anchors.verticalCenter: parent.verticalCenter

				Accessible.ignored: true

				//: LABEL ANDROID IOS
				text: qsTr("Fold in")
				textStyle: Style.text.normal_inverse
			}
		}
	}

	Item {
		id: quitTutorial
		height: quitRow.height + 2 * Constants.component_spacing
		width: quitRow.width
		anchors.top: parent.top

		Accessible.name: quitText.text
		Accessible.onPressAction: if (Qt.platform.os === "ios") baseItem.quitTutorialClicked()

		MouseArea {
			anchors.fill: parent
			preventStealing: true
			onClicked: {
				baseItem.quitTutorialClicked()
			}
		}

		Row {
			id: quitRow
			height: quitText.height
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
			padding: Constants.component_spacing
			spacing: Constants.component_spacing

			GText {
				id: quitText

				anchors.verticalCenter: parent.verticalCenter

				Accessible.ignored: true

				//: LABEL ANDROID IOS
				text: qsTr("Quit tutorial")
				textStyle: Style.text.normal_inverse
			}

			Image {
				anchors.verticalCenter: parent.verticalCenter
				source: "qrc:///images/tutorial/cross.svg"

				height: parent.height
				width: height * (sourceSize.width / sourceSize.height)
				fillMode: Image.PreserveAspectFit
			}
		}
	}
}

