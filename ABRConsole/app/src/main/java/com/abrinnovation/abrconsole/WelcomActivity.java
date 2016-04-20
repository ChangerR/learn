package com.abrinnovation.abrconsole;

import android.content.Intent;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

public class WelcomActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_welcom);
    }

    public void onConnectClick(View view) {
        Log.i("ABRConsole", "connect");
        Intent intent = new Intent(this,ShowActivity.class);
        startActivity(intent);
    }

    public void onContactUsClick(View view) {
        Log.i("ABRConsole","contact us");
        AlertDialog.Builder builder = new AlertDialog.Builder(WelcomActivity.this,R.style.AppCompatAlertDialogStyle);

        builder.setTitle(R.string.contact_us);
        builder.setMessage(R.string.contact_us_msg);
        builder.setPositiveButton(R.string.btn_ok, null);
        builder.show();
    }

    public void onAboutUsClick(View view) {
        Log.i("ABRConsole","about us");
        AlertDialog.Builder builder = new AlertDialog.Builder(WelcomActivity.this,R.style.AppCompatAlertDialogStyle);

        builder.setTitle(R.string.about_us);
        builder.setMessage(R.string.copyright);
        builder.setPositiveButton(R.string.btn_ok,null);
        builder.show();
    }

    public void onTestClick(View view) {
        Log.i("ABRConsole","test button");
        Intent intent = new Intent(this,TestActivity.class);
        startActivity(intent);
    }
}
