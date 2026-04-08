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
		//: ALL_PLATFORMS %1 is replaced with "AusweisApp"
		text: qsTr("Securely identify yourself online with the %1").arg(Qt.application.name)
	}
	GText {
		Layout.topMargin: Style.dimens.text_spacing
		//: ALL_PLATFORMS
		text: qsTr("The eID function allows you to handle administrative or business matters easily and securely online.")
	}
	GText {
		Layout.topMargin: Style.dimens.text_spacing
		text: Style.is_layout_desktop ?
		//: DESKTOP The text between %1 and %2 will be emphasized, %3 is replaced with "AusweisApp"
		qsTr("To identify yourself to a service provider using the %3, you need your %1ID card%2, your %1PIN%2, and an %1NFC-enabled smartphone%2 or a USB card reader.").arg("<b>").arg("</b>").arg(Qt.application.name) :
		//: Mobile The text between %1 and %2 will be emphasized, %3 is replaced with "AusweisApp"
		qsTr("To identify yourself to a service provider using the %3, you need your %1ID card%2, your %1PIN%2, and an %1NFC-enabled smartphone%2.").arg("<b>").arg("</b>").arg(Qt.application.name)
	}
	ButtonBox {
		id: buttonBox

		SecondaryButton {
			Layout.maximumWidth: buttonBox.uniformButtonWidth
			Layout.preferredWidth: buttonBox.uniformButtonWidth
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
		GContinueButton {
			Layout.maximumWidth: buttonBox.uniformButtonWidth
			Layout.preferredWidth: buttonBox.uniformButtonWidth
			//: ALL_PLATFORMS
			text: qsTr("Start setup")

			onClicked: root.continueOnboarding()
		}
	}
	InformativeHint {
		Layout.fillWidth: true
		Layout.topMargin: Style.dimens.pane_spacing

		//: ALL_PLATFORMS
		text: qsTr("The setup is recommended for first-time user. This way it can be ensured that all the technical requirements are met for your online authentication processes.")
		//: ALL_PLATFORMS
		title: qsTr("Are you using the app for the first time?")
	}
}
