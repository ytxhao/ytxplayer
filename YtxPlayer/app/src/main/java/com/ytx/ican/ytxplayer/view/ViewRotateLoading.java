package com.ytx.ican.ytxplayer.view;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.View;
import android.view.animation.LinearInterpolator;

import com.ytx.ican.ytxplayer.R;


public class ViewRotateLoading extends View {

    private static final int DEFAULT_WIDTH = 6;
    private static final int DEFAULT_SHADOW_POSITION = 2;

    private Paint mPaint;

    private RectF loadingRectF;
    private RectF shadowRectF;

    private int topDegree = 10;
    private int bottomDegree = 190;

    private float arc;

    private int width;

    private boolean changeBigger = true;

    private int shadowPosition;

    private int color;

    public ViewRotateLoading(Context context) {
        super(context);
        initView(context, null);
    }

    public ViewRotateLoading(Context context, AttributeSet attrs) {
        super(context, attrs);
        initView(context, attrs);
    }

    public ViewRotateLoading(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initView(context, attrs);
    }

    public interface IRotateLoadingListener {
        void onStop();
    }

    private IRotateLoadingListener rotateLoadingListener;

    public void setRotateLoadingListener(IRotateLoadingListener rotateLoadingListener) {
        this.rotateLoadingListener = rotateLoadingListener;
    }

    private void initView(Context context, AttributeSet attrs) {
        color = Color.WHITE;
        width = dpToPx(context, DEFAULT_WIDTH);
        shadowPosition = dpToPx(getContext(), DEFAULT_SHADOW_POSITION);

        if (null != attrs) {
            TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.RotateLoading);
            color = typedArray.getColor(R.styleable.RotateLoading_loading_color, Color.WHITE);
            width = typedArray.getDimensionPixelSize(R.styleable.RotateLoading_loading_width, dpToPx(context, DEFAULT_WIDTH));
            shadowPosition = typedArray.getInt(R.styleable.RotateLoading_shadow_position, DEFAULT_SHADOW_POSITION);
            typedArray.recycle();
        }

        mPaint = new Paint();
        mPaint.setColor(color);
        mPaint.setAntiAlias(true);
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setStrokeWidth(width);
        mPaint.setStrokeCap(Paint.Cap.ROUND);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);

        arc = 160;

        loadingRectF = new RectF(2 * width, 2 * width, w - 2 * width, h - 2 * width);
        shadowRectF = new RectF(2 * width + shadowPosition, 2 * width + shadowPosition, w - 2 * width + shadowPosition, h - 2 * width + shadowPosition);
    }


    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        mPaint.setColor(Color.parseColor("#1a000000"));
        canvas.drawArc(shadowRectF, topDegree, arc, false, mPaint);
        canvas.drawArc(shadowRectF, bottomDegree, arc, false, mPaint);

        mPaint.setColor(color);
        canvas.drawArc(loadingRectF, topDegree, arc, false, mPaint);
        canvas.drawArc(loadingRectF, bottomDegree, arc, false, mPaint);

        topDegree += 15;
        bottomDegree += 8;
        if (topDegree > 360) {
            topDegree = topDegree - 360;
        }
        if (bottomDegree > 360) {
            bottomDegree = bottomDegree - 360;
        }

        if (changeBigger) {
            if (arc < 160) {
                arc += 2.5;
                invalidate();
            }
        } else {
            if (arc > 10) {
                arc -= 5;
                invalidate();
            }
        }
        if (arc == 160 || arc == 10) {
            changeBigger = !changeBigger;
            invalidate();
        }
    }

    public void start() {
        //startAnimator();
        arc = 160;
        invalidate();
    }

    public void stop() {
        stopAnimator();
        invalidate();
    }

    private void startAnimator() {
        ObjectAnimator scaleXAnimator = ObjectAnimator.ofFloat(this, "scaleX", 0.0f, 1);
        ObjectAnimator scaleYAnimator = ObjectAnimator.ofFloat(this, "scaleY", 0.0f, 1);
        scaleXAnimator.setDuration(300);
        scaleXAnimator.setInterpolator(new LinearInterpolator());
        scaleYAnimator.setDuration(300);
        scaleYAnimator.setInterpolator(new LinearInterpolator());
        AnimatorSet animatorSet = new AnimatorSet();
        animatorSet.playTogether(scaleXAnimator, scaleYAnimator);
        animatorSet.start();
    }

    private void stopAnimator() {
        ObjectAnimator scaleXAnimator = ObjectAnimator.ofFloat(this, "scaleX", 1, 0);
        ObjectAnimator scaleYAnimator = ObjectAnimator.ofFloat(this, "scaleY", 1, 0);
        scaleXAnimator.setDuration(0);
        scaleXAnimator.setInterpolator(new LinearInterpolator());
        scaleYAnimator.setDuration(0);
        scaleYAnimator.setInterpolator(new LinearInterpolator());
        AnimatorSet animatorSet = new AnimatorSet();
        animatorSet.playTogether(scaleXAnimator, scaleYAnimator);
        animatorSet.setStartDelay(500);
        animatorSet.addListener(new Animator.AnimatorListener() {
            @Override
            public void onAnimationStart(Animator animation) {

            }

            @Override
            public void onAnimationEnd(Animator animation) {
                if(rotateLoadingListener != null){
                    rotateLoadingListener.onStop();
                }
            }

            @Override
            public void onAnimationCancel(Animator animation) {

            }

            @Override
            public void onAnimationRepeat(Animator animation) {

            }
        });
        animatorSet.start();
    }


    public int dpToPx(Context context, float dpVal) {
        return (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dpVal, context.getResources().getDisplayMetrics());
    }

}