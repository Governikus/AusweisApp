/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type

BaseOnboardingView {
	id: root

	QtObject {
		id: d

		readonly property bool continuesToAutoStart: numberOfPrompts > 0
		readonly property int numberOfPrompts: Style.is_layout_desktop ? (SettingsModel.autoStartAvailable && !SettingsModel.autoStartSetByAdmin ? 2 : 1) : 0
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ALL_PLATFORMS
		text: qsTr("Requirements not met")
		textStyle: Style.text.headline
	}
	TintableIcon {
		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.pane_spacing
		source: "qrc:///images/info.svg"
		sourceSize.height: Style.dimens.header_icon_size
		tintColor: Style.color.image
	}
	GText {
		Layout.topMargin: Style.dimens.pane_spacing
		//: LABEL ALL_PLATFORMS %1 is replaced with the name "AusweisApp".
		text: qsTr("You cannot use the %1").arg(Qt.application.name)
		textStyle: Style.text.subline
	}
	GText {
		Layout.topMargin: Style.dimens.text_spacing
		//: LABEL ALL_PLATFORMS %1 is replaced with the name "AusweisApp".
		text: qsTr("Unfortunately you do not meet all the requirements necessary to use the %1.").arg(Qt.application.name)
	}
	GText {
		readonly property string abortText: switch (d.numberOfPrompts) {
		case 0:
			//: LABEL ALL_PLATFORMS
			return qsTr("The setup will now be aborted.");
		case 1:
			//: LABEL DESKTOP %1 and %2 are replaced with bold emphasis, %3 with the name "AusweisApp".
			return qsTr("The setup will now be aborted - afterwards %1one prompt regarding personalized settings in the %3%2 will follow.").arg("<b>").arg("</b>").arg(Qt.application.name);
		default:
			//: LABEL DESKTOP %1 and %2 are replaced with bold emphasis, %3 with the name "AusweisApp".
			return qsTr("The setup will now be aborted - afterwards %1two prompts regarding personalized settings in the %3%2 will follow.").arg("<b>").arg("</b>").arg(Qt.application.name);
		}

		//: LABEL ALL_PLATFORMS %1 and %2 are replaced with bold emphasis.
		readonly property string restartOnboardingText: qsTr("You may restart the setup anytime under %1Help > Onboarding%2.").arg("<b>").arg("</b>")

		Layout.topMargin: Style.dimens.text_spacing
		text: abortText + " " + restartOnboardingText
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GButton {
		Layout.alignment: Qt.AlignHCenter
		//LABEL ANDROID IOS
		text: qsTr("Abort setup")
		visible: !d.continuesToAutoStart

		onClicked: root.continueOnboarding()
	}
	GContinueButton {
		Layout.alignment: Qt.AlignHCenter
		visible: d.continuesToAutoStart

		onClicked: root.continueOnboarding()
	}
}
