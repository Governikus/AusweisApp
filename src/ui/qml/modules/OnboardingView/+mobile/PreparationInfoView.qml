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

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.leaveView()
	}

	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ANDROID IOS
		text: qsTr("What is required to use %1?").arg(Qt.application.name)
		textStyle: Style.text.headline
	}
	GText {
		//: LABEL ANDROID IOS
		text: "1. " + qsTr("Your ID card")
		textStyle: Style.text.subline
	}
	GText {
		//: LABEL ANDROID IOS
		text: qsTr("The following documents are allowed:")
	}
	GText {
		readonly property string listelement: "<li><b>%1</b></li>"

		Layout.bottomMargin: Style.dimens.text_spacing
		//: LABEL ANDROID IOS
		text: "<ul>" + listelement.arg(qsTr("german ID card")) +
		//: LABEL ANDROID IOS
		listelement.arg(qsTr("electronic residence permit")) +
		//: LABEL ANDROID IOS
		listelement.arg(qsTr("eID card for EU citizens")) + "</ul>"
	}
	GText {
		//: LABEL ANDROID IOS
		text: "2. " + qsTr("A NFC-enabled smartphone")
		textStyle: Style.text.subline
	}
	GText {
		Layout.bottomMargin: Style.dimens.pane_spacing

		//: LABEL ANDROID IOS  %1 + %2 = Bold Tags
		text: qsTr("The chip in your ID card is read using %1NFC%2. To do this, simply place the ID card on the %1back of the smartphone%2.").arg("<b>").arg("</b>")
	}
	GText {
		//: LABEL ANDROID IOS
		text: "3. " + qsTr("Your PIN")
		textStyle: Style.text.subline
	}
	GText {
		//: LABEL ANDROID IOS  %1 + %2 = Bold Tags, %3 = AusweisApp
		text: qsTr("You have received a one-time PIN, the %1Transport PIN%2, as a letter from your competent authority. You can replace this with a%1 6-digit card PIN%2 in the %3 or at the Citizens' Registration Office. If you do not have a PIN or do not remember your card PIN, click here:").arg("<b>").arg("</b>").arg(Qt.application.name)
	}
	MoreInformationLink {
		Layout.bottomMargin: Style.dimens.pane_spacing * 2
		//: LABEL ANDROID IOS
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

			//: LABEL ANDROID IOS
			continueButtonText: qsTr("Abort setup")
			progress: root.progress

			infoContent: MultiInfoData {
				contentType: MultiInfoData.NO_PIN
			}
			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}

			onContinueClicked: root.abortWithUnknownPin()
		}
	}
}
