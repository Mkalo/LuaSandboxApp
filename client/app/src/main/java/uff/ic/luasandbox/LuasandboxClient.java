package uff.ic.luasandbox;

import android.util.Log;

import org.java_websocket.client.WebSocketClient;
import org.java_websocket.framing.Framedata;
import org.java_websocket.handshake.ServerHandshake;

import java.net.URI;

public class LuasandboxClient extends WebSocketClient {
    private LuasandboxListener eventListener;

    public LuasandboxClient(URI serverURI) {
        super(serverURI);
    }

    public void setEventListener(LuasandboxListener listener) {
        this.eventListener = listener;
    }

    @Override
    public void onOpen(ServerHandshake handshakedata) {
        if (this.eventListener != null) {
            this.eventListener.onConnectionOpen(handshakedata);
        }
    }

    @Override
    public void onMessage(String message) {
        if (this.eventListener != null) {
            this.eventListener.onConnectionMessage(message);
        }
    }

    @Override
    public void onFragment(Framedata fragment) {
        if (this.eventListener != null) {
            this.eventListener.onConnectionFragment(fragment);
        }
    }

    @Override
    public void onClose(int code, String reason, boolean remote) {
        if (this.eventListener != null) {
            this.eventListener.onConnectionClose(code, reason, remote);
        }
    }

    @Override
    public void onError(Exception ex) {
        if (this.eventListener != null) {
            this.eventListener.onConnectionError(ex);
        }
    }
}
