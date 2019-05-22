import QtQuick 2.10

import Governikus.Global 1.0

Rectangle {
	id: baseItem
	height: Math.max(backToMenu.height, quitTutorial.height)

	property alias backRotation: leftArrow.rotation
	property alias backText: menuText.text
	property bool backToMenuActive: true

	signal menuClicked()
	signal quitTutorialClicked()

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

	Item {
		id: backToMenu
		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter
		height: menuRow.height + 2 * Constants.component_spacing
		width: menuRow.width

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

			Text {
				id: menuText
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Fold in") + settingsModel.translationTrigger
				font.family: "Noto Serif"
				font.pixelSize: Constants.tutorial_content_font_size
				color: Constants.white
			}
		}
	}

	Item {
		id: quitTutorial
		height: quitRow.height + 2 * Constants.component_spacing
		width: quitRow.width
		anchors.verticalCenter: parent.verticalCenter

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

			Text {
				id: quitText
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Quit tutorial") + settingsModel.translationTrigger
				font.family: "Noto Serif"
				font.pixelSize: Constants.tutorial_content_font_size
				color: Constants.white
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

