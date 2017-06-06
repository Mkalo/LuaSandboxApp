package uff.ic.luasandbox;

import org.java_websocket.framing.Framedata;
import org.java_websocket.handshake.ServerHandshake;

public interface LuasandboxListener {
    public void onConnectionOpen(ServerHandshake handshakedata);
    public void onConnectionMessage(String message);
    public void onConnectionFragment(Framedata fragment);
    public void onConnectionClose(int code, String reason, boolean remote);
    public void onConnectionError(Exception ex);
}
