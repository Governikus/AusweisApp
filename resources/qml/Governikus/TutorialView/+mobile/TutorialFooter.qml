/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	id: baseItem

	property alias backRotation: backToMenu.imageRotation
	property alias backText: backToMenu.text
	property bool backToMenuActive: true

	signal menuClicked()
	signal quitTutorialClicked()

	height: plugin.safeAreaMargins.bottom + buttonRow.height

	color: footer.color
	Behavior on color { ColorAnimation { duration: Constants.animation_duration } }

	state: "showOnlyQuit"
	states: [
		State { name: "showBothOptions"; when: baseItem.backToMenuActive
			PropertyChanges { target: backToMenu; opacity: 1 }
			PropertyChanges { target: quitTutorial; Layout.alignment: Qt.AlignVCenter | Qt.AlignRight; }
		},
		State { name: "showOnlyQuit"; when: !baseItem.backToMenuActive
			PropertyChanges { target: backToMenu; opacity: 0 }
			PropertyChanges { target: quitTutorial; Layout.alignment: Qt.AlignCenter; }
		}
	]
	transitions: [
		Transition {
			PropertyAnimation { target: backToMenu; duration: 500; property: "opacity"}
			PropertyAnimation { target: quitTutorial; duration: 500; easing.type: Easing.InOutQuad; property: "Layout.alignment"}
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

	RowLayout {
		id: buttonRow

		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
			leftMargin: Constants.component_spacing
			rightMargin: Constants.component_spacing
		}

		spacing: Constants.component_spacing

		TutorialFooterButton {
			id: backToMenu

			Layout.fillWidth: true
			Layout.maximumWidth: implicitWidth * opacity
			Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

			visible: opacity > 0.0

			//: LABEL ANDROID IOS
			text: qsTr("Fold in")
			direction: Qt.LeftToRight
			imageSource: "qrc:///images/tutorial/arrows.svg"
			imageRotation: -90

			onClicked: baseItem.menuClicked()
		}

		TutorialFooterButton {
			id: quitTutorial

			Layout.fillWidth: true
			Layout.maximumWidth: implicitWidth

			//: LABEL ANDROID IOS
			text: qsTr("Quit tutorial")
			direction: Qt.RightToLeft
			imageSource: "qrc:///images/tutorial/cross.svg"

			onClicked: baseItem.quitTutorialClicked()
		}
	}
}
