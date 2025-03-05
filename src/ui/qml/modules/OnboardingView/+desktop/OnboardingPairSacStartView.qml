/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type

BaseOnboardingView {
	id: root

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Back
		startEnabled: false

		onNavigationActionClicked: root.leaveView()
	}

	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		horizontalAlignment: Text.AlignHCenter
		//: LABEL DESKTOP
		text: qsTr("Set up smartphone as card reader")
		textStyle: Style.text.headline
	}
	GridLayout {
		columnSpacing: Style.dimens.pane_spacing
		columns: 2
		rowSpacing: Style.dimens.pane_spacing

		QRCode {
		}
		PairSacPrerequisites {
		}
		PairSacStart {
			Layout.column: 1
			Layout.row: 1
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GButton {
		Layout.alignment: Qt.AlignHCenter
		icon.source: "qrc:///images/material_arrow_right.svg"
		layoutDirection: Qt.RightToLeft
		//: LABEL DESKTOP
		text: qsTr("Continue pairing")
		tintIcon: true

		onClicked: root.continueOnboarding()
	}

	component AusweisAppLogo: RowLayout {
		TintableIcon {
			Layout.alignment: Qt.AlignVCenter
			Layout.rowSpan: 2
			source: "qrc:/images/npa.svg"
			sourceSize.height: Style.dimens.large_icon_size
			tintEnabled: false
		}
		ColumnLayout {
			spacing: 0

			GText {
				font.weight: Font.Bold
				text: "AusweisApp Bund"
				verticalAlignment: Text.AlignBottom
			}
			GText {
				text: "Governikus GmbH & Co. KG"
				verticalAlignment: Text.AlignTop
			}
		}
	}
	component PairSacPrerequisites: ColumnLayout {
		spacing: Style.dimens.groupbox_spacing

		GText {
			//: LABEL DESKTOP %1 is replaced with the name AusweisApp
			text: "1. " + qsTr("Install %1 on your smartphone").arg(Qt.application.name)
			textStyle: Style.text.subline
		}
		GText {

			//: LABEL DESKTOP %1 and %2 are replaced with bold highlighting, %3 with the name AusweisApp
			text: qsTr("Scan the QR code, search your %1app store%2 for %3 or open the following link in your smartphone:").arg("<b>").arg("</b>").arg(Qt.application.name)
		}
		GText {
			readonly property string link: "https://www.ausweisapp.bund.de/%1/aa2/download".arg(SettingsModel.language)

			text: "<a href=\"%1\">%1</a>".arg(link)
		}
		AusweisAppLogo {
		}
	}
	component PairSacStart: ColumnLayout {
		spacing: Style.dimens.groupbox_spacing

		GText {
			//: LABEL DESKTOP %1 is replaced with the name AusweisApp
			text: "2. " + qsTr("Open %1 on your smartphone").arg(Qt.application.name)
			textStyle: Style.text.subline
		}
		GText {
			//: LABEL DESKTOP %1/%3 is replaced with bold highlighting, %2 with the name AusweisApp
			text: qsTr("Select \"%1Set up %2%3\" there.").arg("<b>").arg(Qt.application.name).arg("</b>")
		}
		GText {
			//: LABEL DESKTOP %1/%2 is replaced with bold highlighting
			text: qsTr("You will then be asked whether you want to set up a PC/Mac too. Select the option \"%1Yes, I want to use a PC/Mac too%2\" here. You now select \"%1Continue pairing%2\" on both devices.").arg("<b>").arg("</b>")
		}
	}
	component QRCode: Image {
		source: "qrc:///images/desktop/qrcode_download_%1.png".arg(SettingsModel.language)
		sourceSize.height: Style.dimens.header_icon_size
	}
}
