/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.PersonalizationModel 1.0
import Governikus.Type.SettingsModel 1.0

SectionPage {
	id: root

	navigationAction: NavigationAction { action: NavigationAction.Action.Back; onClicked: pop() }

	//: LABEL ANDROID IOS
	title: qsTr("Smart-eID")
	titleBarColor: Style.color.accent_smart

	sectionPageFlickable: contentItem
	GFlickableColumnLayout {
		id: contentItem

		readonly property var minIconHeight: Style.dimens.medium_icon_size
		readonly property var maxIconHeight: Style.dimens.header_icon_size

		anchors.fill: parent

		minimumContentHeight: implicitContentHeight - (maxIconHeight - minIconHeight)
		maximumContentWidth: Style.dimens.max_text_width
		spacing: Constants.component_spacing

		TintableIcon {
			Layout.fillHeight: true
			Layout.alignment: Qt.AlignHCenter
			Layout.minimumHeight: contentItem.minIconHeight
			Layout.maximumHeight: contentItem.maxIconHeight
			Layout.preferredHeight: contentItem.maxIconHeight

			source: "qrc:///images/identify.svg"
			sourceSize.height: contentItem.maxIconHeight
			tintColor: Style.color.accent

			PkiSwitch {
				anchors.fill: parent
			}
		}

		GPane {
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			GText {
				width: parent.width

				//: LABEL ANDROID IOS
				text: qsTr("You are about to renew your Smart-eID. In order to proceed, you need your ID card, your six-digit ID card PIN and an internet connection.")
			}

			GText {
				width: parent.width

				//: LABEL ANDROID IOS
				text: qsTr("Please note that your current Smart-eID is invalidated during the process and will not be usable until the update process is completed.")
				font.bold: true
			}

			PrivacyStatement {
				width: parent.width

				smart: true
			}
		}

		GSpacer { Layout.fillHeight: true }

		GButton {
			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("Renew Smart-eID")
			icon.source: "qrc:///images/identify.svg"
			buttonColor: SettingsModel.useSelfauthenticationTestUri ? Style.color.warning_text : Style.color.accent

			onClicked: PersonalizationModel.startWorkflow()
		}
	}
}
