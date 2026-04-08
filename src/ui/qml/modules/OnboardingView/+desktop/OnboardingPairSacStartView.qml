/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
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
		navigationAction: NavigationAction.Action.Back
		startEnabled: false

		onNavigationActionClicked: root.leaveView()
	}

	Heading {
		Layout.bottomMargin: Style.dimens.pane_spacing
		//: DESKTOP
		text: qsTr("Set up smartphone as card reader")
	}
	GridLayout {
		Layout.bottomMargin: Style.dimens.pane_spacing
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
	GContinueButton {
		//: DESKTOP
		text: qsTr("Continue pairing")

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
				font.weight: Style.font.bold
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

		Subheading {
			//: DESKTOP %1 is replaced with the name AusweisApp
			text: "1. " + qsTr("Install %1 on your smartphone").arg(Qt.application.name)
		}
		GText {
			//: DESKTOP %1 and %2 are replaced with bold highlighting, %3 with the name AusweisApp
			text: qsTr("Scan the QR code, search your %1app store%2 for %3 or open the following link in your smartphone:").arg("<b>").arg("</b>").arg(Qt.application.name)
		}
		GText {
			readonly property string downloadLink: "https://www.ausweisapp.bund.de/%1/aa2/download".arg(SettingsModel.language)

			text: "<a href=\"%1\">%1</a>".arg(downloadLink)
		}
		AusweisAppLogo {
		}
	}
	component PairSacStart: ColumnLayout {
		spacing: Style.dimens.groupbox_spacing

		Subheading {
			//: DESKTOP %1 is replaced with the name AusweisApp
			text: "2. " + qsTr("Open %1 on your smartphone").arg(Qt.application.name)
		}
		GText {
			//: DESKTOP %1/%3 is replaced with bold highlighting, %2 with the name AusweisApp
			text: qsTr("Select \"%1Set up %2%3\" there.").arg("<b>").arg(Qt.application.name).arg("</b>")
		}
		GText {
			//: DESKTOP %1/%2 is replaced with bold highlighting
			text: qsTr("You will then be asked whether you want to set up a PC/Mac too. Select the option \"%1Yes, I want to use a PC/Mac too%2\" here. You now select \"%1Continue pairing%2\" on both devices.").arg("<b>").arg("</b>")
		}
	}
	component QRCode: Image {
		source: "qrc:///images/desktop/qrcode_download_%1.png".arg(SettingsModel.language)
		sourceSize.height: Style.dimens.header_icon_size
	}
}
