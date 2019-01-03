import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0

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
			desc: QT_TR_NOOP("Identify")
			condition: "identify"
		}

		ListElement {
			image: "qrc:///images/android/navigation/anbieter.svg"
			desc: QT_TR_NOOP("Provider")
			condition: "provider"
		}

		ListElement {
			image: "qrc:///images/android/navigation/verlauf.svg"
			desc: QT_TR_NOOP("History")
			condition: "history"
		}

		ListElement {
			image: "qrc:///images/android/navigation/pin.svg"
			desc: QT_TR_NOOP("PIN Management")
			condition: "pin"
		}

		ListElement {
			image: "qrc:///images/android/navigation/remoteleser.svg"
			desc: QT_TR_NOOP("Smartphone as card reader")
			condition: "remoteservice"
		}

		ListElement {
			image: "qrc:///images/android/navigation/tutorial.svg"
			desc: QT_TR_NOOP("Tutorial")
			condition: "tutorial"
		}

		ListElement {
			image: "qrc:///images/android/navigation/support.svg"
			desc: QT_TR_NOOP("Help & Feedback")
			condition: "feedback"
		}

		ListElement {
			image: "qrc:///images/android/navigation/faq.svg"
			desc: QT_TR_NOOP("Information")
			condition: "information"
		}

		ListElement {
			image: "qrc:///images/zahnraeder.svg"
			desc: QT_TR_NOOP("Developer options")
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
			text: qsTr(desc) + settingsModel.translationTrigger
			onClicked: {
				navigationController.currentIndex = index
				navigationController.state = condition
				navigationController.close()
			}
			// Hide developer options if we are not using developer build (debug build)
			visible: condition !== "developeroptions" || plugin.developerBuild
		}
	}

	LocationButton {
		id: lang_de

		language: "de"
		name: "DE"
		image: "qrc:///images/location_flag_de.svg"

		anchors.margins: Constants.component_spacing
		anchors.bottom: parent.bottom
		anchors.right: lang_en.left
	}

	LocationButton {
		id: lang_en

		language: "en"
		name: "EN"
		image: "qrc:///images/location_flag_en.svg"

		anchors.margins: Constants.component_spacing
		anchors.bottom: parent.bottom
		anchors.right: parent.right
	}
}
