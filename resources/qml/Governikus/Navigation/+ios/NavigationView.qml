/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	id: content

	height: parent.height
	width: parent.width

	ListModel {
		id: navModel

		ListElement {
			image: "qrc:///images/iOS/tabBar/ausweisen.svg"
			desc: QT_TR_NOOP("Identify")
			condition: "identify"
		}

		ListElement {
			image: "qrc:///images/main_provider.svg"
			desc: QT_TR_NOOP("Provider")
			condition: "provider"
		}

		ListElement {
			image: "qrc:///images/iOS/tabBar/remoteleser.svg"
			desc: QT_TR_NOOP("Remote")
			condition: "remoteservice"
		}

		ListElement {
			image: "qrc:///images/main_pin.svg"
			desc: QT_TR_NOOP("PIN")
			condition: "pin"
		}

		ListElement {
			image: "qrc:///images/iOS/tabBar/more.svg"
			desc: QT_TR_NOOP("More")
			condition: "more"
		}
	}

	GSeparator {
		id: topBorderLine
		width: parent.width
	}

	RowLayout {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.top: topBorderLine.bottom

		Repeater {
			id: repeater
			model: navModel

			delegate: NavigationItem {
				Layout.fillWidth: true
				Layout.fillHeight: true

				Accessible.ignored: content.Accessible.ignored

				source: image
				text: qsTr(desc) + SettingsModel.translationTrigger
				selected: baseItem.state === condition
				onClicked: {
					if (baseItem.state === condition) {
						baseItem.reselectedState()
					} else {
						baseItem.state = condition
					}
				}
			}
		}
	}
}
