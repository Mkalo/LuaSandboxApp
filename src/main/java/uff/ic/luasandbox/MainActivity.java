package uff.ic.luasandbox;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;


public class MainActivity extends AppCompatActivity {
    public EditText mainEditText;
    public String fileAtual;
    public String path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/codigos";
    private static final int PICKFILE_RESULT_CODE = 1;
    private static final int CREATEFILE_RESULT_CODE = 2;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        final File dir = new File(path);
        dir.mkdirs();

        mainEditText = (EditText) findViewById(R.id.mainEditText);

        Button openButton = (Button) findViewById(R.id.openButton);
        openButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                intent.setType("plain/text");
                startActivityForResult(intent,PICKFILE_RESULT_CODE);
            }
        });

        Button saveButton = (Button) findViewById(R.id.saveButton);
        saveButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(fileAtual!=null) {

                    File file = new File(path+"/" + fileAtual);
                    String[] saveText = String.valueOf(mainEditText.getText()).split(System.getProperty("line.separator"));

                    Toast.makeText(getApplicationContext(), "Saved at "+fileAtual, Toast.LENGTH_LONG).show();

                    Save(file, saveText);
                }else{
                    chamaNew();

                }
            }
        });

        Button newButton = (Button) findViewById(R.id.newButton);
        newButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                chamaNew();
            }
        });

        Button runButton = (Button) findViewById(R.id.runButton);
        runButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent it = new Intent(MainActivity.this, OutputActivity.class);
                it.putExtra("SCRIPT", mainEditText.getText().toString());
                startActivity(it);
            }
        });

        Button settingsButton = (Button) findViewById(R.id.settingsButton);
        settingsButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent it = new Intent(MainActivity.this, SettingsActivity.class);
                startActivity(it);
            }
        });
    }
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // TODO Auto-generated method stub
        switch(requestCode){
            case PICKFILE_RESULT_CODE:
                if(resultCode==RESULT_OK){
                    File file = new File(data.getData().getPath());
                    String [] loadText = Load(file);
                    fileAtual=data.getData().getLastPathSegment();

                    String finalString = "";

                    for (int i = 0; i < loadText.length; i++)
                    {
                        finalString += loadText[i] + System.getProperty("line.separator");
                    }

                    mainEditText.setText(finalString);

                }
                break;
            case CREATEFILE_RESULT_CODE:
                if(resultCode==RESULT_OK){
                    File file = new File(path+"/"+data.getExtras().getString("RESULT"));
                    fileAtual=data.getExtras().getString("RESULT");
                    String [] saveText = String.valueOf(mainEditText.getText()).split(System.getProperty("line.separator"));
                    Toast.makeText(getApplicationContext(), file.getName(), Toast.LENGTH_LONG).show();

                    Save (file, saveText);
                }
                break;

        }
    }

    private void chamaNew() {
        Intent intent = new Intent(MainActivity.this,SaveAs.class);
        startActivityForResult(intent,CREATEFILE_RESULT_CODE);
    }
    public static void Save(File file, String[] data)
    {
        FileOutputStream fos = null;
        try
        {
            fos = new FileOutputStream(file);
        }
        catch (FileNotFoundException e) {e.printStackTrace();}
        try
        {
            try
            {
                for (int i = 0; i<data.length; i++)
                {
                    fos.write(data[i].getBytes());
                    if (i < data.length-1)
                    {
                        fos.write("\n".getBytes());
                    }
                }
            }
            catch (IOException e) {e.printStackTrace();}
        }
        finally
        {
            try
            {
                fos.close();
            }
            catch (IOException e) {e.printStackTrace();}
        }
    }
    public static String[] Load(File file)
    {
        FileInputStream fis = null;
        try
        {
            fis = new FileInputStream(file);
        }
        catch (FileNotFoundException e) {e.printStackTrace();}
        InputStreamReader isr = new InputStreamReader(fis);
        BufferedReader br = new BufferedReader(isr);

        String test;
        int anzahl=0;
        try
        {
            while ((test=br.readLine()) != null)
            {
                anzahl++;
            }
        }
        catch (IOException e) {e.printStackTrace();}

        try
        {
            fis.getChannel().position(0);
        }
        catch (IOException e) {e.printStackTrace();}

        String[] array = new String[anzahl];

        String line;
        int i = 0;
        try
        {
            while((line=br.readLine())!=null)
            {
                array[i] = line;
                i++;
            }
        }
        catch (IOException e) {e.printStackTrace();}
        return array;
    }
}
