/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.View 1.0
import QtQuick 2.15

SectionPage {

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("Identify")
		helpTopic: "selfauthentication"
	}

	Keys.onReturnPressed: startWorkflowButton.onClicked()
	Keys.onEnterPressed: startWorkflowButton.onClicked()

	Column {
		width: Style.dimens.max_text_width
		anchors.centerIn: parent

		spacing: Constants.pane_spacing
		Item {
			height: childrenRect.height
			width: parent.width

			Image {
				id: useNpa

				width: ApplicationModel.scaleFactor * 400
				height: width
				anchors.left: parent.left

				fillMode: Image.PreserveAspectFit
				source: "qrc:///images/siteWithLogo.png"
				mipmap: true
			}

			GText {
				anchors.verticalCenter: useNpa.verticalCenter
				anchors.leftMargin: Constants.component_spacing
				anchors.rightMargin: Constants.component_spacing
				anchors.left: useNpa.right
				anchors.right: parent.right

				activeFocusOnTab: true
				//: LABEL DESKTOP A11y description of eID logo displayed next to the logo itself
				Accessible.name: qsTr("You can use your ID card anywhere you find the logo of the electronic identification function.")

				textStyle: Style.text.header

				//: LABEL DESKTOP
				text: qsTr("You can use your ID card anywhere you see this logo.")

				FocusFrame {}
			}
		}
		GPane {
			id: textPane

			anchors {
				left: parent.left
				right: parent.right
			}

			GText {
				width: parent.width

				activeFocusOnTab: true

				textStyle: Style.text.normal
				//: LABEL DESKTOP
				text: qsTr("Use the button \"See my personal data\" to start the self-authentication service of the manufacturer of the %1 to display the data stored in the chip of your ID card.")
					.arg(Qt.application.name)

				FocusFrame {}
			}

			PrivacyStatement {
				width: parent.width

				activeFocusOnTab: true
				textStyle: Style.text.normal

				FocusFrame {}
			}

			GButton {
				id: startWorkflowButton

				anchors.right: parent.right

				icon.source: "qrc:///images/identify.svg"
				tintIcon: true
				buttonColor: SettingsModel.useSelfauthenticationTestUri ? Constants.red : Style.color.button
				//: LABEL DESKTOP
				text: qsTr("See my personal data")
				onClicked: SelfAuthModel.startWorkflow()
			}
		}
	}

}
