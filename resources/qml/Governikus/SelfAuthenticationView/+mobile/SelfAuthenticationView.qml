/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.AuthView
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.ReaderPlugIn
import Governikus.Type.SettingsModel
import Governikus.Type.SelfAuthModel
import Governikus.Type.UiModule

FlickableSectionPage {
	id: root

	property bool autoInsertCard: false
	property bool hideTechnologySwitch: false
	property var initialPlugIn

	signal back

	spacing: Constants.pane_spacing
	//: LABEL ANDROID IOS
	title: qsTr("Identify")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.back()
	}

	Component {
		id: authView

		AuthView {
			autoInsertCard: root.autoInsertCard
			hideTechnologySwitch: root.hideTechnologySwitch
			initialPlugIn: root.initialPlugIn

			Component.onCompleted: SelfAuthModel.startWorkflow(false)
			onShowChangePinView: {
				show(UiModule.PINMANAGEMENT);
				popAll();
			}
			onWorkflowFinished: popAll()
		}
	}
	TintableIcon {
		Layout.alignment: Qt.AlignHCenter
		source: "qrc:///images/mydata.svg"
		sourceSize.height: Style.dimens.large_icon_size
		tintColor: Style.color.control

		PkiSwitch {
			anchors.fill: parent
			//: LABEL ANDROID IOS
			functionName: qsTr("Self-authentication")
		}
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.leftMargin: Constants.pane_padding
		Layout.rightMargin: Constants.pane_padding
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ANDROID IOS
		text: qsTr("Use the button \"See my personal data\" to start the self-authentication service of the manufacturer of the %1 to display the data stored in the chip of your ID card.").arg(Qt.application.name)
		width: parent.width
		wrapMode: Text.WordWrap
	}
	PrivacyStatement {
		Layout.fillWidth: true
		Layout.leftMargin: Constants.pane_padding
		Layout.rightMargin: Constants.pane_padding
		horizontalAlignment: Text.AlignHCenter
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GridLayout {
		Layout.fillWidth: true
		Layout.leftMargin: Constants.pane_padding
		Layout.rightMargin: Layout.leftMargin
		columnSpacing: rowSpacing
		columns: 2
		rowSpacing: Constants.groupbox_spacing
		rows: 2

		GText {
			Layout.columnSpan: 2
			//: LABEL ANDROID IOS
			text: qsTr("Hint")
			textStyle: Style.text.subline
		}
		Image {
			fillMode: Image.PreserveAspectFit
			source: "qrc:///images/siteWithLogo_%1.svg".arg(Style.currentTheme.name)
			sourceSize.height: Style.dimens.large_icon_size
		}
		GText {
			//: LABEL ANDROID IOS A11y description of eID logo displayed next to the logo itself
			Accessible.name: qsTr("You can use your ID card anywhere you find the logo of the electronic identification function.")

			//: LABEL ANDROID IOS
			text: qsTr("You can use your ID card anywhere you see this logo.")
		}
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GButton {
		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Constants.component_spacing
		buttonColor: SettingsModel.useSelfauthenticationTestUri ? Style.color.text_warning : Style.color.control
		icon.source: "qrc:///images/identify.svg"
		//: LABEL ANDROID IOS
		text: qsTr("See my personal data")
		tintIcon: true

		onClicked: push(authView)
	}
}
