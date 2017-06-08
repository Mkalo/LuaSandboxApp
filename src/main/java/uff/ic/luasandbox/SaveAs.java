package uff.ic.luasandbox;

import android.content.Intent;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

import java.io.File;

public class SaveAs extends AppCompatActivity {
    public EditText novoNome,lista;
    public Button criar;
    public String path = Environment.getExternalStorageDirectory().getAbsolutePath()+"/codigos/";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_save_as);

        novoNome = (EditText) findViewById(R.id.editText2);
        criar = (Button) findViewById(R.id.criar);
        File dir = new File(path);
        dir.mkdirs();

        final ListView lv;
        lv = (ListView)findViewById(R.id.fileList);

        File[] filelist = dir.listFiles();
        String[] theNamesOfFiles = new String[filelist.length];
        for (int i = 0; i < theNamesOfFiles.length; i++) {
            theNamesOfFiles[i] = filelist[i].getName();
        }
        lv.setAdapter(new ArrayAdapter<String>(this,
                R.layout.mylist,R.id.Itemname, theNamesOfFiles));
        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent, View v,
                                    int position, long id) {
                // Clicking on items
                String name_of_file =       lv.getItemAtPosition(position).toString();
                Intent it = new Intent();
                it.putExtra("RESULT",name_of_file);
                setResult(RESULT_OK, it);
                finish();


            }
        });
        criar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(!(novoNome.getText().toString().trim().equals(""))) {

                    Intent it = new Intent();
                    it.putExtra("RESULT", novoNome.getText() + ".txt");
                    setResult(RESULT_OK, it);
                    finish();
                }else{
                    Toast.makeText(getApplicationContext(), "digite um nome", Toast.LENGTH_LONG).show();
                }
            }
        });






    }
}
