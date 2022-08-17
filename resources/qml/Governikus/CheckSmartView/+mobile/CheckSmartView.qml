/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
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

	signal startAuth()
	signal checkDevice()
	signal runSmartSetup()

	navigationAction: NavigationAction { action: NavigationAction.Action.Back; onClicked: pop() }

	//: LABEL ANDROID IOS
	title: qsTr("Check Smart-eID")
	titleBarColor: Style.color.accent_smart

	Component {
		id: checkSmartEidResultView

		CheckSmartResultView {
			result: SmartModel.smartState

			onStartAuth: root.startAuth()
			onCheckDevice: root.checkDevice()
			onRunSmartSetup: root.runSmartSetup()
			onCancelClicked: {
				setLockedAndHidden(false)
				popAll()
			}
		}
	}

	sectionPageFlickable: contentItem
	GFlickableColumnLayout {
		id: contentItem

		readonly property var minIconHeight: Style.dimens.medium_icon_size
		readonly property var maxIconHeight: Style.dimens.header_icon_size

		anchors.fill: parent

		minimumContentHeight: implicitContentHeight - (maxIconHeight - minIconHeight)
		maximumContentWidth: Style.dimens.max_text_width
		spacing: Constants.component_spacing

		TintableIcon {
			Layout.fillHeight: true
			Layout.alignment: Qt.AlignHCenter
			Layout.minimumHeight: contentItem.minIconHeight
			Layout.maximumHeight: contentItem.maxIconHeight
			Layout.preferredHeight: contentItem.maxIconHeight

			source: "qrc:///images/mobile/phone_smart.svg"
			sourceSize.height: contentItem.maxIconHeight
			tintEnabled: false
		}

		GPane {
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			GText {
				width: parent.width

				//: LABEL ANDROID IOS
				text: qsTr("Your device needs to meet the technical requirements to use the Smart-eID functionality.")
			}

			GText {
				width: parent.width

				//: LABEL ANDROID IOS
				text: qsTr("Check here if your device is suitable to set up a Smart-eID.")
			}
		}

		GSpacer { Layout.fillHeight: true }

		GButton {
			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("Start check")
			icon.source: "qrc:///images/mobile/device.svg"

			onClicked: {
				setLockedAndHidden()
				push(checkSmartEidResultView)
			}
		}
	}
}
