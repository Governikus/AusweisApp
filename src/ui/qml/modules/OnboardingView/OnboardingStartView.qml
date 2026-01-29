/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Governikus.Animations
import Governikus.Global
import Governikus.Style

BaseOnboardingView {
	id: root

	signal skipOnboarding

	//: ALL_PLATFORMS Headline
	title: qsTr("Setup")

	Heading {
		//: ALL_PLATFORMS
		text: qsTr("Set up the eID function")
		wrapMode: Text.WordWrap
	}
	SiteWithLogoAnimation {
		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.pane_spacing
		sourceSize.height: Style.dimens.header_icon_size
	}
	Subheading {
		Layout.topMargin: Style.dimens.pane_spacing
		//: ALL_PLATFORMS
		text: qsTr("What can I do with it?")
	}
	GText {
		readonly property string technologyString: Style.is_layout_desktop ?
		//: DESKTOP The text between %1 and %2 will be emphasized.
		qsTr("All you need is your %1ID card%2, %1PIN%2 and a %1NFC-enabled smartphone%2 or an %1USB card reader%2.").arg("<b>").arg("</b>") :
		//: MOBILE The text between %1 and %2 will be emphasized.
		qsTr("All you need is your %1ID card%2, %1PIN%2 and a %1NFC-enabled smartphone%2.").arg("<b>").arg("</b>")

		Layout.topMargin: Style.dimens.text_spacing
		//: ALL_PLATFORMS The text between %1 and %2 will be emphasized.
		text: qsTr("It allows you to identify yourself %1securely%2 on the internet. You can easily take care of %1administrative or business matters%2 online.").arg("<b>").arg("</b>") + " " + technologyString
	}
	GInformativeButton {
		Layout.fillWidth: true
		Layout.topMargin: Style.dimens.pane_spacing
		//: ALL_PLATFORMS
		description: qsTr("I use the eID function for the 1st time")
		//: ALL_PLATFORMS %1 will be replaced with the name of the application
		text: qsTr("Set up %1").arg(Qt.application.name)

		onClicked: root.continueOnboarding()
	}
	GInformativeButton {
		Layout.fillWidth: true
		Layout.topMargin: Style.dimens.pane_spacing
		//: ALL_PLATFORMS
		description: qsTr("I already used the eID function before")
		isPane: true
		//: ALL_PLATFORMS
		text: qsTr("Skip setup")

		onClicked: root.push(skipConfirmation)

		Component {
			id: skipConfirmation

			SkipOnboardingConfirmation {
				title: root.title

				onContinueOnboarding: {
					pop();
					root.continueOnboarding();
				}
				onLeaveView: pop()
				onSkipOnboarding: {
					pop();
					root.skipOnboarding();
				}
			}
		}
	}
}
