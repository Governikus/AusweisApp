/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


Rectangle {
	id: content
	height: appWindow.height - Style.dimens.titlebar_height
	width: 250
	color: Style.color.background

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


	GListView {
		id: listView
		anchors.fill: parent
		boundsBehavior: Flickable.StopAtBounds
		model: navModel
		currentIndex: navigationController.currentIndex

		highlight: Rectangle {
			color: "black"
			opacity: 0.1
			height: 45
			width: content.width
			y: listView.currentItem.y
		}
		highlightFollowsCurrentItem: false

		delegate: NavigationItem {
			height: 45
			width: content.width
			source: image
			text: qsTr(desc) + SettingsModel.translationTrigger
			onClicked: {
				if (navigationController.state === condition) {
					navigationController.reselectedState()
				} else {
					navigationController.currentIndex = index
					navigationController.state = condition
				}
				navigationController.close()
			}
			// Hide developer options if we are not using developer build (debug build)
			visible: condition !== "developeroptions" || plugin.developerBuild
		}
	}

	LocationButton {
		id: lang_de

		anchors.margins: Constants.component_spacing
		anchors.bottomMargin: plugin.safeAreaMargins.bottom + Constants.component_spacing
		anchors.bottom: parent.bottom
		anchors.right: lang_en.left

		Accessible.name: qsTr("Set language to german") + SettingsModel.translationTrigger

		language: "de"
		name: "DE"
		image: "qrc:///images/location_flag_de.svg"
	}

	LocationButton {
		id: lang_en

		anchors.margins: Constants.component_spacing
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		anchors.bottomMargin: plugin.safeAreaMargins.bottom + Constants.component_spacing

		Accessible.name: qsTr("Set language to english") + SettingsModel.translationTrigger

		language: "en"
		name: "EN"
		image: "qrc:///images/location_flag_en.svg"
	}
}
