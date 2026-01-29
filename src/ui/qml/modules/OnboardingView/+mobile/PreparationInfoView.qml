/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
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

	Heading {
		Layout.bottomMargin: Style.dimens.pane_spacing
		//: MOBILE
		text: qsTr("What is required to use %1?").arg(Qt.application.name)
	}
	Subheading {
		//: MOBILE
		text: "1. " + qsTr("Your ID card")
	}
	GText {
		//: MOBILE
		text: qsTr("The following documents are allowed:")
	}
	GText {
		readonly property string listelement: "<li><b>%1</b></li>"

		Layout.bottomMargin: Style.dimens.text_spacing
		//: MOBILE
		text: "<ul>" + listelement.arg(qsTr("german ID card")) +
		//: MOBILE
		listelement.arg(qsTr("electronic residence permit")) +
		//: MOBILE
		listelement.arg(qsTr("eID card for EU citizens")) + "</ul>"
	}
	Subheading {
		//: MOBILE
		text: "2. " + qsTr("A NFC-enabled smartphone")
	}
	GText {
		Layout.bottomMargin: Style.dimens.pane_spacing

		//: MOBILE  %1 + %2 = Bold Tags
		text: qsTr("The chip in your ID card is read using %1NFC%2. To do this, simply place the ID card on the %1back of the smartphone%2.").arg("<b>").arg("</b>")
	}
	Subheading {
		//: MOBILE
		text: "3. " + qsTr("Your PIN")
	}
	GText {
		//: MOBILE  %1 + %2 = Bold Tags, %3 = AusweisApp
		text: qsTr("You have received a one-time PIN, the %1Transport PIN%2, as a letter from your competent authority. You can replace this with a%1 6-digit card PIN%2 in the %3 or at the Citizens' Registration Office. If you do not have a PIN or do not remember your card PIN, tap here:").arg("<b>").arg("</b>").arg(Qt.application.name)
	}
	MoreInformationLink {
		Layout.bottomMargin: Style.dimens.pane_spacing
		//: MOBILE
		text: qsTr("I can't recall my PIN")

		onClicked: root.push(multiInfoViewNoPin)
	}
	GContinueButton {
		onClicked: root.continueOnboarding()
	}
	Component {
		id: multiInfoViewNoPin

		MultiInfoView {
			//: MOBILE
			continueButtonText: qsTr("Abort setup")
			progress: root.progress

			infoContent: MultiInfoData {
				contentType: MultiInfoData.Type.NO_PIN
			}
			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}

			onContinueClicked: root.abortWithUnknownPin()
		}
	}
}
