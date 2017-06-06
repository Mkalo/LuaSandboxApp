package uff.ic.luasandbox;

import android.app.Activity;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.widget.TextView;

import org.java_websocket.framing.Framedata;
import org.java_websocket.handshake.ServerHandshake;

import java.net.URI;
import java.net.URISyntaxException;

public class OutputActivity extends AppCompatActivity {
    private LuasandboxClient client;
    private String codeToExecute;
    private TextView outputTextView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_output);

        Toolbar outputToolbar = (Toolbar) findViewById(R.id.outputToolbar);
        setSupportActionBar(outputToolbar);
        ActionBar actionBar = getSupportActionBar();
        actionBar.setHomeButtonEnabled(true);
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setDisplayShowHomeEnabled(false);

        this.outputTextView = (TextView) findViewById(R.id.outputTextView);
        Bundle extras = getIntent().getExtras();
        this.codeToExecute = extras.getString("SCRIPT");

        try {
            this.client = new LuasandboxClient(new URI("ws://10.0.2.2:6000"));
        } catch (URISyntaxException e) {
            Log.d("CLIENT", e.getMessage());
            this.finish();
            return;
        }
        this.client.setEventListener(new LuasandboxListener() {
            @Override
            public void onConnectionOpen(ServerHandshake handshakedata) {
                Log.d("CLIENT", "opened connection");
                client.send(codeToExecute);
            }

            @Override
            public void onConnectionMessage(String message) {
                Log.d("CLIENT", "received: " + message);
                final String msg = message;
                outputTextView.post(new Runnable() {
                    @Override
                    public void run() {
                        outputTextView.setText(msg);
                    }
                });
            }

            @Override
            public void onConnectionFragment(Framedata fragment) {
                Log.d("CLIENT", "received fragment: " + new String(fragment.getPayloadData().array()));
            }

            @Override
            public void onConnectionClose(int code, String reason, boolean remote) {
                Log.d("CLIENT", "Connection closed by " + (remote ? "remote peer" : "us" ));
            }

            @Override
            public void onConnectionError(Exception ex) {
                Log.d("CLIENT", ex.getMessage());
            }
        });
        this.client.connect();

    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                this.finish();
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }
}
