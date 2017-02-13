package org.bulllord.util;
import android.app.Activity;
import android.view.View;
import android.view.KeyEvent;
import android.view.ViewGroup;
import android.widget.EditText;
import android.content.Context;
import android.view.inputmethod.InputMethodManager;
import android.graphics.Color;
import android.os.Build;
import android.view.inputmethod.EditorInfo;
import android.text.InputType;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
public class BLInput {
    protected static Activity mSingleton;
    protected static BLText mInput;
    public static native void textChanged(String text);
    public static class BLText extends EditText{		
		public BLText(Context context) {
			super(context);
			setSingleLine();
			setBackgroundColor(Color.TRANSPARENT);
			setImeOptions(EditorInfo.IME_ACTION_NONE | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
			setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_NORMAL);
			addTextChangedListener(new TextWatcher() {
				@Override
				public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
				@Override
				public void onTextChanged(CharSequence s, int start, int before, int count) {
					final String text = new String(s.toString());
					textChanged(text);
				}
				@Override
				public void afterTextChanged(Editable s) {}
			});
		}		
		@Override
		public boolean onKeyDown(final int pKeyCode, final KeyEvent pKeyEvent) {   
			switch (pKeyCode) 
			{
				case KeyEvent.KEYCODE_BACK:
					hideTextInput();
					return true;
				default:
					return super.onKeyDown(pKeyCode, pKeyEvent);
			}
		}
    }
    public BLInput(Context context) {
		mSingleton = (Activity)context;
		mSingleton.getWindow().getDecorView().getViewTreeObserver().addOnGlobalLayoutListener(new OnGlobalLayoutListener(){
			@Override
			public void onGlobalLayout(){
				View view = mSingleton.getWindow().getDecorView();
				int visibility = 0;
				if (Build.VERSION.SDK_INT >= 14)
					visibility |= View.SYSTEM_UI_FLAG_LOW_PROFILE;
				if (Build.VERSION.SDK_INT >= 16)
					visibility |= View.SYSTEM_UI_FLAG_FULLSCREEN;
				if (Build.VERSION.SDK_INT >= 19)
					visibility |= View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
				view.setSystemUiVisibility(visibility);
			}
		});
    }
	public static void showTextInput() {
		 mSingleton.runOnUiThread(new Runnable() {
							@Override
							public void run() {
								mInput.setVisibility(View.VISIBLE);
								mInput.requestFocus();
								InputMethodManager imm = (InputMethodManager) mSingleton.getSystemService(Context.INPUT_METHOD_SERVICE);
								imm.showSoftInput(mInput, 0);
							}
						});
    }
	public static void hideTextInput() {
		 mSingleton.runOnUiThread(new Runnable() {
							@Override
							public void run() {
								mInput.setText(""); 
								mInput.setVisibility(View.GONE);
								InputMethodManager imm = (InputMethodManager) mSingleton.getSystemService(Context.INPUT_METHOD_SERVICE);
								imm.hideSoftInputFromWindow(mInput.getWindowToken(), 0);
							}
						});
    }   
	public static void addTextEdit() {
		 mSingleton.runOnUiThread(new Runnable() {
							@Override
							public void run() {
								mInput = new BLText((Context)mSingleton);
								ViewGroup.LayoutParams param = new ViewGroup.LayoutParams(0, 0);
								mSingleton.addContentView(mInput, param);
								mInput.setVisibility(View.GONE);
								InputMethodManager imm = (InputMethodManager) mSingleton.getSystemService(Context.INPUT_METHOD_SERVICE);
								imm.hideSoftInputFromWindow(mInput.getWindowToken(), 0);
							}
						});
	}
}
