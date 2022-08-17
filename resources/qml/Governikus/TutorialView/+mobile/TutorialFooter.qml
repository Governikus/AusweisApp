/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	id: baseItem

	property alias backRotation: leftArrow.rotation
	property alias backText: menuText.text
	property bool backToMenuActive: true

	signal menuClicked()
	signal quitTutorialClicked()

	height: plugin.safeAreaMargins.bottom + Math.max(backToMenu.height, quitTutorial.height)

	color: footer.color
	Behavior on color { ColorAnimation { duration: Constants.animation_duration } }

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

	Item {
		id: backToMenu
		anchors.left: parent.left
		anchors.top: parent.top
		height: menuRow.height + 2 * Constants.component_spacing
		width: menuRow.width

		Accessible.name: menuText.text
		Accessible.onPressAction: baseItem.menuClicked()

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
		Accessible.onPressAction: baseItem.quitTutorialClicked()

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
