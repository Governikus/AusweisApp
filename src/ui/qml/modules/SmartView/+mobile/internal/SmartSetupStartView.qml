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
	title: qsTr("Smart-eID")

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
		Layout.fillWidth: true

		GText {
			//: MOBILE
			text: qsTr("You are about to set up a Smart-eID on your device. In order to proceed, you need your ID card, your 6-digit ID card PIN and an internet connection.")
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
		text: qsTr("Set up Smart-eID")

		onClicked: PersonalizationModel.startWorkflow()
	}
}
