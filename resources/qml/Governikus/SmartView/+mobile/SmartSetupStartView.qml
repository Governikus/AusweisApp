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

	//: LABEL ANDROID IOS
	title: qsTr("Smart-eID")
	titleBarColor: Style.color.accent_smart

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: Constants.component_spacing
		spacing: Constants.component_spacing

		TintableIcon {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.maximumHeight: Style.dimens.header_icon_size
			source: "qrc:///images/identify.svg"
			tintColor: Style.color.accent

			PkiSwitch {
				anchors.fill: parent
			}
		}
		GPane {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width

			GText {

				//: LABEL ANDROID IOS
				text: qsTr("You are about to set up a Smart-eID on your device. In order to proceed, you need you ID card, your six-digit ID card PIN and an internet connection.")
				width: parent.width
			}
			PrivacyStatement {
				smart: true
				width: parent.width
			}
		}
		GSpacer {
			Layout.fillHeight: true
		}
		GButton {
			Layout.alignment: Qt.AlignHCenter
			buttonColor: SettingsModel.useSelfauthenticationTestUri ? Style.color.warning_text : Style.color.accent
			icon.source: "qrc:///images/identify.svg"

			//: LABEL ANDROID IOS
			text: qsTr("Set up Smart-eID")

			onClicked: PersonalizationModel.startWorkflow()
		}
	}
}
