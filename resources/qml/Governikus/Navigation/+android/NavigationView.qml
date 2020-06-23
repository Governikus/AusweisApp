/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	id: content

	property var navigationController: null

	height: appWindow.height - Style.dimens.titlebar_height
	width: 250

	ListModel {
		id: navModel

		ListElement {
			image: "qrc:///images/android/navigation/ausweisen.svg"
			desc: QT_TR_NOOP("Identify")
			condition: "identify"
		}

		ListElement {
			image: "qrc:///images/main_provider.svg"
			desc: QT_TR_NOOP("Provider")
			condition: "provider"
		}

		ListElement {
			image: "qrc:///images/android/navigation/verlauf.svg"
			desc: QT_TR_NOOP("History")
			condition: "history"
		}

		ListElement {
			image: "qrc:///images/main_pin.svg"
			desc: QT_TR_NOOP("PIN Management")
			condition: "pin"
		}

		ListElement {
			image: "qrc:///images/android/navigation/remoteleser.svg"
			desc: QT_TR_NOOP("Remote service")
			condition: "remoteservice"
		}

		ListElement {
			image: "qrc:///images/settings_icon.svg"
			desc: QT_TR_NOOP("Settings")
			condition: "settings"
			section: "settings"
		}

		ListElement {
			image: "qrc:///images/android/navigation/support.svg"
			desc: QT_TR_NOOP("Help & Feedback")
			condition: "feedback"
			section: "settings"
		}
	}

	GListView {
		id: listView

		anchors.fill: parent

		boundsBehavior: Flickable.StopAtBounds
		model: navModel
		currentIndex: navigationController.currentIndex
		section.property: "section"
		section.delegate: Item {
			height: Constants.component_spacing
			width: parent.width

			GSeparator {
				width: parent.width
				anchors.verticalCenter: parent.verticalCenter
			}
		}
		highlight: Rectangle {
			y: listView.currentItem.y
			height: 45
			width: content.width

			color: "#1A000000" //10% transparent black

			GSeparator {
				anchors {
					top: parent.top
					left: parent.left
					right: parent.right
				}
			}

			GSeparator {
				anchors {
					bottom: parent.bottom
					left: parent.left
					right: parent.right
				}
			}
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
		}
	}
}
