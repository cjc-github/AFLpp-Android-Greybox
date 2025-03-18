package com.example.test_demo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.example.test_demo.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'test_demo' library on application startup.
    static {
        System.loadLibrary("test_demo");
    }

    /**
     * A native method that is implemented by the 'test_demo' native library,
     * which is packaged with this application.
     */
    public native void stringFromJNI(String data);

    private ActivityMainBinding binding;
    private EditText editView;
    private Button button;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        editView = findViewById(R.id.editView);
        button = findViewById(R.id.button);

        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String inputText = editView.getText().toString(); // 获取 TextView 的内容
                stringFromJNI(inputText); // 调用 native 方法

                Toast.makeText(MainActivity.this, "success", Toast.LENGTH_SHORT).show();
            }
        });

    }
}