/*
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
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
import Governikus.TutorialView 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.UiModule 1.0

Item {
	id: baseItem

	property int activeModule
	readonly property var visibleLoader: visibleChildren[0]
	readonly property var visibleItem: if (visibleLoader) visibleLoader.item
	readonly property var currentSectionPage: if (visibleItem) visibleItem.currentSectionPage
	readonly property int startupModule: SettingsModel.startupModule
	readonly property bool initialViewsReady: identifyView.isLoaded && initialView.isLoaded
	readonly property bool ready: initialViewsReady && checkIDCardView.isLoaded && providerView.isLoaded && changePinView.isLoaded &&
								  historyView.isLoaded && settingsView.isLoaded && helpView.isLoaded && remoteView.isLoaded
	readonly property var initialView:
		startupModule === UiModule.CHECK_ID_CARD ? checkIDCardView :
		startupModule === UiModule.IDENTIFY ? identifyView :
		startupModule === UiModule.PROVIDER ? providerView :
		startupModule === UiModule.PINMANAGEMENT ? changePinView :
		startupModule === UiModule.HISTORY ? historyView :
		startupModule === UiModule.REMOTE_SERVICE ? remoteView :
		startupModule === UiModule.SETTINGS ? settingsView :
		startupModule === UiModule.HELP ? helpView :
		startupModule === UiModule.TUTORIAL ? tutorialView :
		mainView

	ContentAreaLoader {
		id: identifyView

		contentArea: baseItem
		module: UiModule.IDENTIFY

		active: true
		asynchronous: false
		sourceComponent: Component {
			TabBarView {
				sourceComponent: IdentifyView {}
			}
		}
	}

	ContentAreaLoader {
		id: mainView

		contentArea: baseItem
		module: UiModule.DEFAULT

		sourceComponent: Component {
			TabBarView {
				sourceComponent: MainView {}
			}
		}
	}

	ContentAreaLoader {
		id: checkIDCardView

		contentArea: baseItem
		module: UiModule.CHECK_ID_CARD

		sourceComponent: Component {
			TabBarView {
				sourceComponent: CheckIDCardView {}
			}
		}
	}

	ContentAreaLoader {
		id: providerView

		contentArea: baseItem
		module: UiModule.PROVIDER

		sourceComponent: Component {
			TabBarView {
				sourceComponent: ProviderView {}
			}
		}
	}

	ContentAreaLoader {
		id: changePinView

		contentArea: baseItem
		module: UiModule.PINMANAGEMENT

		sourceComponent: Component {
			TabBarView {
				sourceComponent: ChangePinView {}
			}
		}
	}

	ContentAreaLoader {
		id: historyView

		contentArea: baseItem
		module: UiModule.HISTORY

		sourceComponent: Component {
			TabBarView {
				sourceComponent: HistoryView {}
			}
		}
	}

	ContentAreaLoader {
		id: remoteView

		contentArea: baseItem
		module: UiModule.REMOTE_SERVICE

		sourceComponent: Component {
			TabBarView {
				sourceComponent: RemoteServiceView {}
			}
		}
	}

	ContentAreaLoader {
		id: settingsView

		contentArea: baseItem
		module: UiModule.SETTINGS

		sourceComponent: Component {
			TabBarView {
				sourceComponent: SettingsView {}
			}
		}
	}

	ContentAreaLoader {
		id: helpView

		contentArea: baseItem
		module: UiModule.HELP

		sourceComponent: Component {
			TabBarView {
				sourceComponent: MoreView {}
			}
		}
	}

	ContentAreaLoader {
		id: tutorialView

		contentArea: baseItem
		module:  UiModule.TUTORIAL

		active: contentArea.activeModule === module || startupModule === module
		sourceComponent: Component {
			TabBarView {
				sourceComponent: TutorialView {
					onLeave: {
						navBar.show(UiModule.DEFAULT)
						SettingsModel.startupModule = UiModule.DEFAULT
					}
				}
			}
		}
	}

}
