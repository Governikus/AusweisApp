/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.CheckIDCardView 1.0
import Governikus.ChangePinView 1.0
import Governikus.MainView 1.0
import Governikus.MoreView 1.0
import Governikus.IdentifyView 1.0
import Governikus.ProviderView 1.0
import Governikus.HistoryView 1.0
import Governikus.SettingsView 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.TutorialView 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.View 1.0

Item {
	id: baseItem

	property bool ready: SettingsModel.showSetupAssistantOnStart ? tutorialView.status == Loader.Ready : mainView.status == Loader.Ready
	readonly property var visibleLoader: visibleChildren[0]
	readonly property var visibleItem: if (visibleLoader) visibleLoader.item
	readonly property var currentSectionPage: if (visibleItem) visibleItem.currentSectionPage

	Loader {
		id: mainView

		anchors.fill: parent
		visible: baseItem.state === "main"

		active: baseItem.ready || visible
		asynchronous: true
		sourceComponent: Component {
			TabBarView {
				sourceComponent: MainView {}
			}
		}
	}

	Loader {
		anchors.fill: parent
		visible: baseItem.state === "checkIDCard"

		active: baseItem.ready
		asynchronous: true
		sourceComponent: Component {
			TabBarView {
				sourceComponent: CheckIDCardView {}
			}
		}
	}

	Loader {
		anchors.fill: parent
		visible: baseItem.state === "identify"

		active: baseItem.ready
		asynchronous: true
		sourceComponent: Component {
			TabBarView {
				sourceComponent: IdentifyView {}
			}
		}
	}

	Loader {
		anchors.fill: parent
		visible: baseItem.state === "provider"

		active: baseItem.ready
		asynchronous: true
		sourceComponent: Component {
			TabBarView {
				sourceComponent: ProviderView {}
			}
		}
	}

	Loader {
		anchors.fill: parent
		visible: baseItem.state === "pin"

		active: baseItem.ready
		asynchronous: true
		sourceComponent: Component {
			TabBarView {
				sourceComponent: ChangePinView {}
			}
		}
	}

	Loader {
		anchors.fill: parent
		visible: baseItem.state === "history"

		active: baseItem.ready
		asynchronous: true
		sourceComponent: Component {
			TabBarView {
				sourceComponent: HistoryView {}
			}
		}
	}

	Loader {
		anchors.fill: parent
		visible: baseItem.state === "remoteservice"

		active: baseItem.ready
		asynchronous: true
		sourceComponent: Component {
			TabBarView {
				sourceComponent: RemoteServiceView {}
			}
		}
	}

	Loader {
		anchors.fill: parent
		visible: baseItem.state === "settings"

		active: baseItem.ready
		asynchronous: true
		sourceComponent: Component {
			TabBarView {
				sourceComponent: SettingsView {}
			}
		}
	}

	Loader {
		anchors.fill: parent
		visible: baseItem.state === "help"

		active: baseItem.ready
		asynchronous: true
		sourceComponent: Component {
			TabBarView {
				sourceComponent: MoreView {}
			}
		}
	}

	Loader {
		id: tutorialView

		anchors.fill: parent
		visible: baseItem.state === "tutorial"

		active: SettingsModel.showSetupAssistantOnStart
		asynchronous: true
		sourceComponent: Component {
			TabBarView {
				sourceComponent: TutorialView {
					onLeave: {
						navBar.showMain()
						SettingsModel.showSetupAssistantOnStart = false
					}
				}
			}
		}
	}

}
