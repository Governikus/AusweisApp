/*
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0

import QtWinExtras 1.0


TaskbarButton {
	progress.minimum: 0
	progress.maximum: 100
	progress.value: AuthModel.progressValue
	progress.visible: ApplicationModel.currentWorkflow === "selfauthentication" || ApplicationModel.currentWorkflow === "authentication"
}
