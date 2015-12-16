package com.abrinnovation.abrconsole;

import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

public class ShowActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_show);
    }

    public void onAboutUsClick(View view) {
        Log.i("ABRConsole", "about us");
        AlertDialog.Builder builder = new AlertDialog.Builder(ShowActivity.this,R.style.AppCompatAlertDialogStyle);

        builder.setTitle(R.string.about_us);
        builder.setMessage(R.string.copyright);
        builder.setPositiveButton(R.string.btn_ok, null);
        builder.show();
    }
}
