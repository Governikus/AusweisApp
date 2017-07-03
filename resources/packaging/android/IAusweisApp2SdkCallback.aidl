package com.governikus.ausweisapp2;

interface IAusweisApp2SdkCallback {
    void sessionIdGenerated(String pSessionId, boolean pIsSecureSessionId);
    void receive(String pJson);
}
