/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: content

	height: parent.height
	width: parent.width

	ListModel {
		id: navModel

		ListElement {
			image: "qrc:///images/mobile/material_home.svg"
			desc: QT_TR_NOOP("Start")
			condition: "main"
		}

		ListElement {
			image: "qrc:///images/history.svg"
			desc: QT_TR_NOOP("History")
			condition: "history"
		}

		ListElement {
			image: "qrc:///images/mobile/material_specific_phone.svg"
			desc: QT_TR_NOOP("Remote")
			condition: "remoteservice"
		}

		ListElement {
			image: "qrc:///images/material_settings.svg"
			desc: QT_TR_NOOP("Settings")
			condition: "settings"
		}

		ListElement {
			image: "qrc:///images/material_help.svg"
			desc: QT_TR_NOOP("Help")
			condition: "help"
		}
	}

	GSeparator {
		id: topBorderLine
		width: parent.width
	}

	RowLayout {
		anchors {
			left: parent.left
			right: parent.right
			bottom: parent.bottom
			top: topBorderLine.bottom
			leftMargin: Style.dimens.navigation_bar_padding
			rightMargin: Style.dimens.navigation_bar_padding
			bottomMargin: Constants.is_layout_android ? Style.dimens.navigation_bar_padding : Style.dimens.navigation_bar_text_padding
			topMargin: Style.dimens.navigation_bar_padding
		}

		Repeater {
			id: repeater

			model: navModel

			delegate: NavigationItem {
				Layout.fillWidth: true
				Layout.fillHeight: true

				Accessible.ignored: content.Accessible.ignored

				source: image
				text: qsTr(desc)
				selected: baseItem.state === condition ||
						  (baseItem.state === "identify" || baseItem.state === "pin" || baseItem.state === "provider" || baseItem.state === "checkIDCard") && condition === "main"
				onClicked: {
					baseItem.resetContentArea()
					if (baseItem.state !== condition) {
						baseItem.state = condition
					}
				}
			}
		}
	}
}
