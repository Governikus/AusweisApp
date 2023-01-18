/*
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: root
	sectionPageFlickable: contentItem
	//: LABEL ANDROID IOS
	title: qsTr("Identify")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: show(UiModule.DEFAULT)
	}

	GFlickableColumnLayout {
		id: contentItem

		readonly property var maxIconHeight: Style.dimens.header_icon_size
		readonly property var minIconHeight: Style.dimens.medium_icon_size

		anchors.fill: parent
		maximumContentWidth: Style.dimens.max_text_width
		minimumContentHeight: implicitContentHeight - (maxIconHeight - minIconHeight)
		spacing: 0

		TintableIcon {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.maximumHeight: contentItem.maxIconHeight
			Layout.minimumHeight: contentItem.minIconHeight
			source: "qrc:///images/mydata.svg"
			sourceSize.height: contentItem.maxIconHeight
			tintColor: Style.color.accent

			PkiSwitch {
				anchors.fill: parent
			}
		}
		GPane {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			Layout.topMargin: Constants.component_spacing

			GText {
				//: LABEL ANDROID IOS
				text: qsTr("Use the button \"See my personal data\" to start the self-authentication service of the manufacturer of the %1 to display the data stored in the chip of your ID card.").arg(Qt.application.name)
				width: parent.width
				wrapMode: Text.WordWrap
			}
			PrivacyStatement {
				width: parent.width
			}
		}
		GSpacer {
			Layout.fillHeight: true
		}
		Hint {
			Layout.fillWidth: true
			Layout.topMargin: Constants.component_spacing

			RowLayout {
				spacing: Constants.text_spacing
				width: parent.width

				Image {
					Layout.fillWidth: true
					Layout.preferredHeight: Style.dimens.medium_icon_size
					asynchronous: true
					fillMode: Image.PreserveAspectFit
					source: "qrc:///images/siteWithLogo.png"
					sourceSize.height: Style.dimens.large_icon_size
				}
				GText {

					//: LABEL ANDROID IOS A11y description of eID logo displayed next to the logo itself
					Accessible.name: qsTr("You can use your ID card anywhere you find the logo of the electronic identification function.")
					Layout.fillWidth: true

					//: LABEL ANDROID IOS
					text: qsTr("You can use your ID card anywhere you see this logo.")
				}
			}
		}
		GButton {
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.component_spacing
			buttonColor: SettingsModel.useSelfauthenticationTestUri ? Style.color.warning_text : Style.color.accent
			icon.source: "qrc:///images/identify.svg"
			//: LABEL ANDROID IOS
			text: qsTr("See my personal data")
			tintIcon: true

			onClicked: SelfAuthModel.startWorkflow()
		}
	}
}
