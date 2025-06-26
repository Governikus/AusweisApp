/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.MultiInfoView
import Governikus.Style
import Governikus.TitleBar

BaseOnboardingView {
	id: root

	signal abortWithUnknownPin

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Back
		startEnabled: false

		onNavigationActionClicked: root.leaveView()
	}

	Keys.onEnterPressed: root.continueOnboarding()
	Keys.onEscapePressed: root.leaveView()
	Keys.onReturnPressed: root.continueOnboarding()

	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		horizontalAlignment: Text.AlignHCenter
		//: LABEL DESKTOP
		text: qsTr("What is required to use %1?").arg(Qt.application.name)
		textStyle: Style.text.headline
	}
	GText {
		//: LABEL DESKTOP
		text: "1. " + qsTr("Your ID card")
		textStyle: Style.text.subline
	}
	GText {
		Layout.bottomMargin: Style.dimens.pane_spacing
		//: LABEL DESKTOP %1 + %2 = Bold Tags
		text: qsTr("You could either use the %1German ID card%2, an %1electronic residence permit%2 or the %1eID card for EU citizens%2.").arg("<b>").arg("</b>")
	}
	GText {
		//: LABEL DESKTOP
		text: "2. " + qsTr("A smartphone or a card reader")
		textStyle: Style.text.subline
	}
	GText {
		Layout.bottomMargin: Style.dimens.pane_spacing
		//: LABEL DESKTOP %1 = Application Name, %2 + %3 = Bold Tags
		text: qsTr("You may use your %2NFC-enabled smartphone%3. To do this you will have to install %1 on this smartphone. Alternatively you may use an %2USB card reader with your PC%3.").arg(Qt.application.name).arg("<b>").arg("</b>")
	}
	GText {
		//: LABEL DESKTOP
		text: "3. " + qsTr("Your PIN")
		textStyle: Style.text.subline
	}
	GText {
		//: LABEL DESKTOP  %1 + %2 = Bold Tags, %3 = AusweisApp
		text: qsTr("You have received a one-time PIN, the %1Transport PIN%2, as a letter from your competent authority. You can replace this with a%1 6-digit card PIN%2 in the %3 or at the Citizens' Registration Office. If you do not have a PIN or do not remember your card PIN, click here:").arg("<b>").arg("</b>").arg(Qt.application.name)
	}
	MoreInformationLink {
		Layout.bottomMargin: Style.dimens.pane_spacing * 2
		//: LABEL DESKTOP
		text: qsTr("I can't recall my PIN")

		onClicked: root.push(multiInfoView)
	}
	GContinueButton {
		onClicked: root.continueOnboarding()
	}
	Component {
		id: multiInfoView

		MultiInfoView {
			id: infoView

			//: LABEL DESKTOP
			continueButtonText: qsTr("Abort setup")
			progress: root.progress

			infoContent: MultiInfoData {
				contentType: MultiInfoData.NO_PIN
			}
			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back
				startEnabled: false

				onNavigationActionClicked: root.pop()
			}

			onContinueClicked: root.abortWithUnknownPin()
		}
	}
}
