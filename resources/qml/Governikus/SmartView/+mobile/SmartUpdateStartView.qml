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
	sectionPageFlickable: contentItem

	//: LABEL ANDROID IOS
	title: qsTr("Smart-eID")
	titleBarColor: Style.color.accent_smart

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}

	GFlickableColumnLayout {
		id: contentItem

		readonly property var maxIconHeight: Style.dimens.header_icon_size
		readonly property var minIconHeight: Style.dimens.medium_icon_size

		anchors.fill: parent
		maximumContentWidth: Style.dimens.max_text_width
		minimumContentHeight: implicitContentHeight - (maxIconHeight - minIconHeight)
		spacing: Constants.component_spacing

		TintableIcon {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.maximumHeight: contentItem.maxIconHeight
			Layout.minimumHeight: contentItem.minIconHeight
			Layout.preferredHeight: contentItem.maxIconHeight
			source: "qrc:///images/identify.svg"
			sourceSize.height: contentItem.maxIconHeight
			tintColor: Style.color.accent

			PkiSwitch {
				anchors.fill: parent
			}
		}
		GPane {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true

			GText {

				//: LABEL ANDROID IOS
				text: qsTr("You are about to renew your Smart-eID. In order to proceed, you need your ID card, your six-digit ID card PIN and an internet connection.")
				width: parent.width
			}
			GText {
				font.bold: true

				//: LABEL ANDROID IOS
				text: qsTr("Please note that your current Smart-eID is invalidated during the process and will not be usable until the update process is completed.")
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
			text: qsTr("Renew Smart-eID")

			onClicked: PersonalizationModel.startWorkflow()
		}
	}
}
