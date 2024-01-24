/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQml.Models
import Governikus.Global
import Governikus.TitleBar
import Governikus.Type.ApplicationModel
import Governikus.Type.NumberModel
import Governikus.Type.RemoteServiceModel
import Governikus.View

SectionPage {
	id: root

	enum SubView {
		None,
		ConnectSacView
	}

	readonly property int availableReader: ApplicationModel.availableReader
	readonly property int currentView: d.view
	property int lastReaderCount: 0
	property alias paneAnchors: tabbedPane.anchors
	property alias rootEnabled: titleBarAction.rootEnabled

	signal closeView

	titleBarAction: TitleBarAction {
		id: titleBarAction

		rootEnabled: false

		//: LABEL DESKTOP
		text: qsTr("Card Readers")

		customSubAction: NavigationAction {
			onClicked: closeView()
		}

		onClicked: d.view = TabbedReaderView.SubView.None
	}

	Component.onCompleted: lastReaderCount = availableReader
	onAvailableReaderChanged: {
		if (visible && availableReader > lastReaderCount) {
			root.closeView();
		}
		lastReaderCount = availableReader;
	}
	onVisibleChanged: d.view = TabbedReaderView.SubView.None

	QtObject {
		id: d

		property int precedingView
		property int view
	}
	TabbedPane {
		id: tabbedPane

		sectionsModel: [qsTr("Smartphone as card reader"), qsTr("USB card reader")]
		visible: d.view === TabbedReaderView.SubView.None

		contentObjectModel: ObjectModel {
			Component {
				RemoteReaderView {
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
					width: parent.width

					onPairDevice: pDeviceId => {
						if (RemoteServiceModel.rememberServer(pDeviceId)) {
							d.view = TabbedReaderView.SubView.ConnectSacView;
							updateTitleBarActions();
						}
					}
					onUnpairDevice: pDeviceId => RemoteServiceModel.forgetDevice(pDeviceId)
				}
			}
			Component {
				CardReaderView {
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
					width: parent.width
				}
			}
		}

		anchors {
			bottom: parent.bottom
			left: parent.left
			margins: Constants.pane_padding
			right: parent.right
			top: parent.top
		}
	}
	NavigationButton {
		buttonType: NavigationButton.Type.Back
		visible: tabbedPane.visible

		onClicked: root.closeView()

		anchors {
			bottom: parent.bottom
			left: parent.left
			margins: Constants.pane_padding
		}
	}
	ConnectSacView {
		id: connectSacView

		rootEnabled: root.rootEnabled
		visible: d.view === TabbedReaderView.SubView.ConnectSacView

		onCloseView: {
			d.view = d.precedingView;
			updateTitleBarActions();
		}
	}
}
