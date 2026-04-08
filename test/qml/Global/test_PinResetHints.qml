/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtTest
import Governikus.Global
import Governikus.Type

TestCase {
	id: testCase

	function test_load_PinResetHints(data) {
		compare(PinResetInformationModel.resetPinWithPRSHint, PinResetInformationModel.getResetPinWithPRSHint(PinResetInformationModel.hasPinResetService));
		const item = createTemporaryQmlObject("
			import Governikus.Global
			import Governikus.Type
			PinResetHints {
				statusCode: %1
			}
			".arg(data.statusCode), parent);

		let d = findChild(item, "Private");
		verify(d !== null);
		d.hasPRS = data.hasPRS;

		if (data.hasPRS)
			d.resetPinWithPRSHint = PinResetInformationModel.getResetPinWithPRSHint(data.hasPRS);

		const prsHint = findChild(item, "PRSHint");
		verify(prsHint !== null);
		const authorityHint = findChild(item, "AuthorityHint");
		verify(authorityHint !== null);

		compare(item.title, data.title);
		compare(item.title === "", data.statusCode !== GlobalStatusCode.Card_Pin_Deactivated && data.statusCode !== GlobalStatusCode.Card_Puk_Blocked);

		compare(prsHint.title, data.prsHintTitle);
		compare(prsHint.text, data.prsHintText);
		compare(prsHint.text === "", data.hasPRS === false);
		compare(prsHint.buttonText, data.prsHintButtonText);

		compare(authorityHint.title, data.authorityHintTitle);
		compare(authorityHint.text, data.authorityHintText);
		compare(authorityHint.buttonText, data.authorityHintButtonText);

		item.destroy();
	}
	function test_load_PinResetHintsWithCustomTitle() {
		const item = createTemporaryQmlObject("
			import Governikus.Global
			import Governikus.Type
			PinResetHints {
				title: \"Other\"
			}
			", parent);
		compare(item.title, "Other");
		item.destroy();
	}
	function test_load_PinResetHints_data() {
		return [
			{
				"hasPRS": true,
				"statusCode": GlobalStatusCode.Card_Pin_Deactivated,
				"title": "How do I activate the eID function?",
				"prsHintTitle": "Online via PIN Reset Service",
				"prsHintText": "You may request a PIN Reset Letter with a new PIN and it's according activation code on the following website to activate the eID function.",
				"prsHintButtonText": "Request PIN Reset Letter",
				"authorityHintTitle": "At your competent authority",
				"authorityHintText": "You may turn to your competent authority to activate the eID function.",
				"authorityHintButtonText": "Find competent authority"
			},
			{
				"hasPRS": false,
				"statusCode": GlobalStatusCode.Card_Pin_Deactivated,
				"title": "How do I activate the eID function?",
				"prsHintTitle": "Online via PIN Reset Service",
				"prsHintText": "",
				"prsHintButtonText": "Request PIN Reset Letter",
				"authorityHintTitle": "At your competent authority",
				"authorityHintText": "You may turn to your competent authority to activate the eID function.",
				"authorityHintButtonText": "Find competent authority"
			},
			{
				"hasPRS": true,
				"statusCode": GlobalStatusCode.Card_Puk_Blocked,
				"title": "My PUK is used up. How do I set a new PIN?",
				"prsHintTitle": "Online via PIN Reset Service",
				"prsHintText": "You may request a PIN Reset Letter with a new PIN and it's according activation code on the following website.",
				"prsHintButtonText": "Request PIN Reset Letter",
				"authorityHintTitle": "At your competent authority",
				"authorityHintText": "You may turn to the competent authority and reset the ID card PIN there.",
				"authorityHintButtonText": "Find competent authority"
			},
			{
				"hasPRS": false,
				"statusCode": GlobalStatusCode.Card_Puk_Blocked,
				"title": "My PUK is used up. How do I set a new PIN?",
				"prsHintTitle": "Online via PIN Reset Service",
				"prsHintText": "",
				"prsHintButtonText": "Request PIN Reset Letter",
				"authorityHintTitle": "At your competent authority",
				"authorityHintText": "You may turn to the competent authority and reset the ID card PIN there.",
				"authorityHintButtonText": "Find competent authority"
			},
			{
				"hasPRS": true,
				"statusCode": -1,
				"title": "",
				"prsHintTitle": "Online via PIN Reset Service",
				"prsHintText": "You may request a PIN Reset Letter with a new PIN and it's according activation code on the following website.",
				"prsHintButtonText": "Request PIN Reset Letter",
				"authorityHintTitle": "At your competent authority",
				"authorityHintText": "You may turn to the competent authority and reset the ID card PIN there.",
				"authorityHintButtonText": "Find competent authority"
			},
			{
				"hasPRS": false,
				"statusCode": -1,
				"title": "",
				"prsHintTitle": "Online via PIN Reset Service",
				"prsHintText": "",
				"prsHintButtonText": "Request PIN Reset Letter",
				"authorityHintTitle": "At your competent authority",
				"authorityHintText": "You may turn to the competent authority and reset the ID card PIN there.",
				"authorityHintButtonText": "Find competent authority"
			}
		];
	}

	name: "test_PinResetHints"
	visible: true
	when: windowShown
}
