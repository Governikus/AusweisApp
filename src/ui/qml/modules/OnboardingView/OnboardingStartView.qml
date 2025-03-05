/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
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

	//: LABEL ALL_PLATFORMS Headline
	title: qsTr("Setup")

	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter

		//: LABEL ALL_PLATFORMS
		text: qsTr("Set up the eID function")
		textStyle: Style.text.headline
		wrapMode: Text.WordWrap
	}
	SiteWithLogoAnimation {
		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.pane_spacing
		sourceSize.height: Style.dimens.header_icon_size
	}
	GText {
		Layout.topMargin: Style.dimens.pane_spacing
		//: LABEL ALL_PLATFORMS
		text: qsTr("What can I do with it?")
		textStyle: Style.text.subline
	}
	GText {
		readonly property string technologyString: Style.is_layout_desktop ?
		//: LABEL DESKTOP The text between %1 and %2 will be emphasized.
		qsTr("All you need is your %1ID card%2, %1PIN%2 and a %1NFC-enabled smartphone%2 or an %1USB card reader%2.").arg("<b>").arg("</b>") :
		//: LABEL ANDROID IOS The text between %1 and %2 will be emphasized.
		qsTr("All you need is your %1ID card%2, %1PIN%2 and a %1NFC-enabled smartphone%2.").arg("<b>").arg("</b>")

		Layout.topMargin: Style.dimens.text_spacing
		//: LABEL ALL_PLATFORMS The text between %1 and %2 will be emphasized.
		text: qsTr("It allows you to identify yourself %1securely%2 on the internet. You can easily take care of %1administrative or business matters%2 online.").arg("<b>").arg("</b>") + " " + technologyString
	}
	GInformativeButton {
		Layout.fillWidth: true
		Layout.topMargin: Style.dimens.pane_spacing
		//: LABEL ALL_PLATFORMS
		description: qsTr("I use the eID function for the 1st time")
		//: LABEL ALL_PLATFORMS %1 will be replaced with the name of the application
		text: qsTr("Set up %1").arg(Qt.application.name)

		onClicked: root.continueOnboarding()
	}
	GInformativeButton {
		Layout.fillWidth: true
		Layout.topMargin: Style.dimens.pane_spacing
		//: LABEL ALL_PLATFORMS
		description: qsTr("I already used the eID function before")
		isPane: true
		//: LABEL ALL_PLATFORMS
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
