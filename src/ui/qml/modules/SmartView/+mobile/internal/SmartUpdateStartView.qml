/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.TitleBar
import Governikus.Type

FlickableSectionPage {
	id: root

	smartEidUsed: true
	spacing: Style.dimens.pane_spacing

	//: MOBILE
	title: qsTr("Renew the Smart-eID")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}

	TintableIcon {
		Layout.alignment: Qt.AlignHCenter
		source: "qrc:///images/identify.svg"
		sourceSize.height: Style.dimens.header_icon_size
		tintColor: Style.color.image

		PkiSwitch {
			anchors.fill: parent
		}
	}
	GPane {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true

		GText {
			//: MOBILE
			text: qsTr("You are about to renew your Smart-eID. In order to proceed, you need your ID card, your 6-digit ID card PIN and an internet connection.")
		}
		GText {
			font.weight: Style.font.bold

			//: MOBILE
			text: qsTr("Please note that your current Smart-eID is invalidated during the process and will not be usable until the update process is completed.")
		}
		PrivacyStatement {
			smart: true
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GButton {
		//: MOBILE
		Accessible.name: text + ". " + qsTr("Sample card required.")
		Layout.alignment: Qt.AlignHCenter
		buttonColor: SettingsModel.useSelfauthenticationTestUri ? Style.color.error : Style.color.control.background.basic_unchecked
		icon.source: "qrc:///images/identify.svg"
		//: MOBILE
		text: qsTr("Renew Smart-eID")

		onClicked: PersonalizationModel.startWorkflow()
	}
}
