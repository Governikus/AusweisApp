import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.MainView 1.0
import Governikus.IdentifyView 1.0
import Governikus.ProviderView 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.UiModule 1.0

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0


ApplicationWindow {
	readonly property int initialWidth: Utils.dp(1600)
	readonly property int initialHeight: Utils.dp(1200)

	id: appWindow
	visible: true
	width: initialWidth
	height: initialHeight
	title: "AusweisApp2"
	background: Item {
		Rectangle {
			anchors.fill: parent
			color: Constants.background_color
			visible: plugin.useFlatStyleOnDesktop()
		}
		Image {
			anchors.fill: parent
			fillMode: Image.PreserveAspectCrop
			source: visible ? "qrc:///images/desktop/background.png" : ""
			visible: !plugin.useFlatStyleOnDesktop()

			LinearGradient {
				anchors.fill: parent
				start: Qt.point(0, 0)
				end: Qt.point(0, appWindow.height)
				gradient: Gradient {
					GradientStop {
						position: 0
						color: "#AA659bcd"
					}
					GradientStop {
						position: 1
						color: "#FF659bcd"
					}
				}
			}
		}
	}

	onWidthChanged: setScaleFactor();
	onHeightChanged: setScaleFactor();
	function setScaleFactor() {
		ApplicationModel.scaleFactor = Math.min(width / initialWidth, height / initialHeight)
	}

	property int activeView: 0
	function activateView(pName) {
		if (pName < 1) {
			console.warn("Unknown view requested: " + pName)
			return;
		}

		activeView = pName
		titleBar.updateActions()
	}

	onClosing: {
		hide()
		plugin.hide();
	}

	Connections {
		target: plugin
		onFireShowRequest: {
			showNormal()
			raise()
			requestActivate()
			if (pModule === UiModule.IDENTIFY) activateView(SectionPage.Views.Identify)
		}
	}

	Action {
		shortcut: "Ctrl+Alt+R"
		onTriggered: plugin.developerBuild ? plugin.doRefresh() : ""
	}

	menuBar: TitleBar {
		id: titleBar
		contentRoot: contentArea
		onRootClicked: activateView(SectionPage.Views.Main)
		navSuccessor: {
			if (viewMain.visible) {
				return viewMain
			}
			if (viewIdentify.visible) {
				return viewIdentify.tabTarget
			}
			if (viewProvider.visible) {
				return viewProvider
			}
			return null
		}
	}

	Item {
		id: contentArea
		anchors.fill: parent

		MainView {
			id: viewMain

			visible: appWindow.activeView === 0 || appWindow.activeView === SectionPage.Views.Main
			onVisibleChildrenChanged: titleBar.updateActions()
			onNextView: activateView(pName)
			navSuccessor: titleBar
		}

		IdentifyView {
			id: viewIdentify

			visible: appWindow.activeView === SectionPage.Views.Identify
			onNextView: activateView(pName)
			navSuccessor: titleBar
		}

		ProviderView {
			id: viewProvider

			visible: appWindow.activeView === SectionPage.Views.Provider
			onVisibleChildrenChanged: titleBar.updateActions()
			onNextView: activateView(pName)
			navSuccessor: titleBar
		}
	}
}
