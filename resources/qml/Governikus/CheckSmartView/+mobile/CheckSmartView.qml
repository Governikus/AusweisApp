/*
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.ProgressView 1.0
import Governikus.SmartView 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.ReaderScanEnabler 1.0
import Governikus.Type.SmartModel 1.0
import Governikus.Type.UiModule 1.0
import Governikus.View 1.0

SectionPage {
	id: root
	signal checkDevice
	signal runSmartSetup
	signal startAuth

	sectionPageFlickable: contentItem

	//: LABEL ANDROID IOS
	title: qsTr("Check Smart-eID")
	titleBarColor: Style.color.accent_smart

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}

	Component {
		id: checkSmartEidResultView
		CheckSmartResultView {
			result: SmartModel.smartState

			onCancelClicked: {
				setLockedAndHidden(false);
				popAll();
			}
			onCheckDevice: root.checkDevice()
			onRunSmartSetup: root.runSmartSetup()
			onStartAuth: root.startAuth()
		}
	}
	GFlickableColumnLayout {
		id: contentItem

		readonly property var maxIconHeight: Style.dimens.header_icon_size
		readonly property var minIconHeight: Style.dimens.medium_icon_size

		anchors.fill: parent
		maximumContentWidth: Style.dimens.max_text_width
		minimumContentHeight: implicitContentHeight - (maxIconHeight - minIconHeight)
		spacing: Constants.component_spacing

		TintableIcon {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.maximumHeight: contentItem.maxIconHeight
			Layout.minimumHeight: contentItem.minIconHeight
			Layout.preferredHeight: contentItem.maxIconHeight
			source: "qrc:///images/mobile/phone_smart.svg"
			sourceSize.height: contentItem.maxIconHeight
			tintEnabled: false
		}
		GPane {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true

			GText {

				//: LABEL ANDROID IOS
				text: qsTr("Your device needs to meet the technical requirements to use the Smart-eID functionality.")
				width: parent.width
			}
			GText {

				//: LABEL ANDROID IOS
				text: qsTr("Check here if your device is suitable to set up a Smart-eID.")
				width: parent.width
			}
		}
		GSpacer {
			Layout.fillHeight: true
		}
		GButton {
			Layout.alignment: Qt.AlignHCenter
			icon.source: "qrc:///images/mobile/device.svg"

			//: LABEL ANDROID IOS
			text: qsTr("Start check")

			onClicked: {
				setLockedAndHidden();
				push(checkSmartEidResultView);
			}
		}
	}
}
