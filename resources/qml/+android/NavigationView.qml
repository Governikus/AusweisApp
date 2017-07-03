import QtQuick 2.5
import QtQuick.Controls 2.0

import "global"


Rectangle {
	id: content
	height: appWindow.height - Constants.titlebar_height
	width: Utils.dp(250)
	color: Constants.background_color

	property var navigationController: null

	ListModel {
		id: navModel

		ListElement {
			image: "qrc:///images/android/navigation/ausweisen.svg"
			desc: qsTr("Identify")
			condition: "identify"
		}

		ListElement {
			image: "qrc:///images/android/navigation/anbieter.svg"
			desc: qsTr("Provider")
			condition: "provider"
		}

		ListElement {
			image: "qrc:///images/android/navigation/verlauf.svg"
			desc: qsTr("History")
			condition: "history"
		}

		ListElement {
			image: "qrc:///images/android/navigation/pin.svg"
			desc: qsTr("PIN Management")
			condition: "pin"
		}

		ListElement {
			image: "qrc:///images/android/navigation/balloon.svg"
			desc: qsTr("Dialog & Feedback")
			condition: "feedback"
		}

		ListElement {
			image: "qrc:///images/android/navigation/faq.svg"
			desc: qsTr("Info & Help")
			condition: "information"
		}

		ListElement {
			image: "qrc:///images/zahnraeder.svg"
			desc: qsTr("Developer options")
			condition: "developeroptions"
		}
	}


	ListView {
		id: listView
		anchors.fill: parent
		boundsBehavior: Flickable.StopAtBounds
		model: navModel
		currentIndex: navigationController.currentIndex

		highlight: Rectangle {
			color: "black"
			opacity: 0.1
			height: Utils.dp(45)
			width: content.width
			y: listView.currentItem.y
		}
		highlightFollowsCurrentItem: false

		delegate: NavigationItem {
			height: Utils.dp(45)
			width: content.width
			source: image
			text: desc
			onClicked: {
				navigationController.currentIndex = index
				navigationController.state = condition
				navigationController.close()
			}
			// Hide developer options if we are not using developer build (debug build)
			visible: condition !== "developeroptions" || plugin.developerBuild
		}
	}
}
