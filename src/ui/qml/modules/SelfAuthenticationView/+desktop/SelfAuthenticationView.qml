/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type
import Governikus.View

SectionPage {
	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("Identify")
	}

	Keys.onEnterPressed: startWorkflowButton.clicked()
	Keys.onReturnPressed: startWorkflowButton.clicked()

	Column {
		anchors.centerIn: parent
		spacing: Constants.pane_spacing
		width: Style.dimens.max_text_width

		Item {
			anchors.horizontalCenter: parent.horizontalCenter
			height: childrenRect.height
			width: parent.width * 0.8

			Image {
				id: useNpa

				anchors.left: parent.left
				fillMode: Image.PreserveAspectFit
				height: Style.dimens.header_icon_size
				mipmap: true
				source: "qrc:///images/siteWithLogo_%1.svg".arg(Style.currentTheme.name)
				width: height
			}
			GText {
				//: LABEL DESKTOP A11y description of eID logo displayed next to the logo itself
				Accessible.name: qsTr("You can use your ID card anywhere you find the logo of the electronic identification function.")
				activeFocusOnTab: true
				anchors.left: useNpa.right
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
				anchors.verticalCenter: useNpa.verticalCenter

				//: LABEL DESKTOP
				text: qsTr("You can use your ID card anywhere you see this logo.")

				FocusFrame {
				}
			}
		}
		GPane {
			id: textPane

			color: Style.color.paneSublevel.background.basic
			drawShadow: false

			anchors {
				left: parent.left
				right: parent.right
			}
			GText {
				activeFocusOnTab: true
				horizontalAlignment: Text.AlignHCenter
				//: LABEL DESKTOP
				text: qsTr("Use the button \"See my personal data\" to start the self-authentication service of the manufacturer of the %1 to display the data stored in the chip of your ID card.").arg(Qt.application.name)

				FocusFrame {
				}
			}
			PrivacyStatement {
				activeFocusOnTab: true
				horizontalAlignment: Text.AlignHCenter

				FocusFrame {
				}
			}
			GButton {
				id: startWorkflowButton

				Layout.alignment: Qt.AlignHCenter
				buttonColor: SettingsModel.useSelfauthenticationTestUri ? Style.color.error : Style.color.control.background.basic
				icon.source: "qrc:///images/identify.svg"
				//: LABEL DESKTOP
				text: qsTr("See my personal data")
				tintIcon: true

				onClicked: SelfAuthModel.startWorkflow()
			}
		}
	}
}
