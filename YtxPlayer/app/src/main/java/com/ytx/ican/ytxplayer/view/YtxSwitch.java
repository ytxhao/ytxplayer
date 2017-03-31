package com.ytx.ican.ytxplayer.view;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup.LayoutParams;

import com.ytx.ican.ytxplayer.R;


public class YtxSwitch extends View {

    private boolean isChecked;
    private boolean isEnabled = true;      //是否可用

    private int mBmpWidth = 0;
    private int mBmpHeight = 0;

    private OnSwicthClickListener onSwicthClickListener = null;
    private OnSwitchChangedListener mOnSwitchChangedListener = null;

    //开关状态图
    Bitmap mSwitch_off, mSwitch_on,mSwitch_off_disable, mSwitch_on_disable;
    
    public YtxSwitch(Context context)
    {
      this(context, null);
    }

    public YtxSwitch(Context context, AttributeSet attrs)
    {
      super(context, attrs);
      init();
    }

    public YtxSwitch(Context context, AttributeSet attrs, int defStyle)
    {
      super(context, attrs, defStyle);
      init();
    }

    //初始化三幅图片
    private void init() {
        Resources res = getResources();
        mSwitch_off = BitmapFactory.decodeResource(res, R.mipmap.switch_close);
        mSwitch_on = BitmapFactory.decodeResource(res, R.mipmap.switch_open);
        mSwitch_off_disable = BitmapFactory.decodeResource(res, R.mipmap.yh_ptz_icon_switch_off_disable);
        mSwitch_on_disable = BitmapFactory.decodeResource(res, R.mipmap.yh_ptz_icon_switch_on_disable);
        mBmpWidth = mSwitch_on.getWidth();
        mBmpHeight = mSwitch_on.getHeight();
    }
    
    @Override
    public void setLayoutParams(LayoutParams params)
    {
      params.width = mBmpWidth;
      params.height = mBmpHeight;
      super.setLayoutParams(params);
    }
    
    /**
     * 为开关控件设置状态改变监听函数
     * @param onSwitchChangedListener 参见 {@link OnSwitchChangedListener}
     */
    public void setOnSwitchChangedListener(OnSwitchChangedListener onSwitchChangedListener)
    {
      mOnSwitchChangedListener = onSwitchChangedListener;
    }
    
    //设置点击监听器
    public void setOnClickListener(OnSwicthClickListener onSwicthClickListener){
        this.onSwicthClickListener = onSwicthClickListener;
    }
    
    /**
     * 设置开关的状态
     */
    public void setChecked(boolean isChecked)
    {
      this.isChecked = isChecked;
      invalidate();
    }
    
    public boolean isChecked() {
        return isChecked;
    }

    public boolean isEnabled() {
        return isEnabled;
    }

    public void setEnabled(boolean isEnabled) {
        this.isEnabled = isEnabled;
        invalidate();
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh)
    {
      super.onSizeChanged(w, h, oldw, oldh);
    }
    
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getAction();
        switch (action) {
        case MotionEvent.ACTION_UP:
            if(isEnabled){
                isChecked = !isChecked;
                if(mOnSwitchChangedListener != null){
                    mOnSwitchChangedListener.onSwitchChanged(this, isChecked);
                }
                invalidate();
            }else{
                if(onSwicthClickListener != null){
                    onSwicthClickListener.onClickListener(this);
                }
            }
            break;
        }
        return true;
    }
    
    @Override
    protected void onDraw(Canvas canvas)
    {
      super.onDraw(canvas);
      if(!isChecked){
          if(isEnabled){
              drawBitmap(canvas, null, null, mSwitch_off);
          }else{
            drawBitmap(canvas, null, null, mSwitch_off_disable);
          }
      }else{
          if(isEnabled){
              drawBitmap(canvas, null, null, mSwitch_on);
          }else{
              drawBitmap(canvas, null, null, mSwitch_on_disable);
          }
      }
    }

    public void drawBitmap(Canvas canvas, Rect src, Rect dst, Bitmap bitmap)
    {
      dst = (dst == null ? new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight()) : dst);
      Paint paint = new Paint();
      canvas.drawBitmap(bitmap, src, dst, paint);
    }

    public static interface OnSwitchChangedListener
    {
      /**
       * 状态改变 回调函数
       * @param status  true表示打开 false表示关闭
       */
      public abstract void onSwitchChanged(YtxSwitch obj, boolean status);
    }
    
    public static interface OnSwicthClickListener{
        //点击监听器
        public abstract void onClickListener(View v);
    }
    
}
