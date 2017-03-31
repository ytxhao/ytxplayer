package com.ytx.ican.ytxplayer.view;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.drawable.Drawable;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.ytx.ican.media.player.pragma.YtxLog;
import com.ytx.ican.ytxplayer.R;
import com.ytx.ican.ytxplayer.utils.ScreenUtil;

public class LabelLayout extends LinearLayout {

    private static final int EDGE_DISTANCE = 18;
    private View dividerViewBottom,dividerViewTop,indicatorView,descriptionView;
    private ImageView ivIcon;
    private TextView tvTitle,tvSubtitle;
    private LinearLayout llTitle;

    private boolean isDescriptionTextView;
    
    public LabelLayout(Context context) {
        this(context,null);
    }

    public LabelLayout(Context context, AttributeSet attrs) {
        this(context, attrs,0);
    }

    public LabelLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        setOrientation(LinearLayout.VERTICAL);
        setMinimumHeight(ScreenUtil.dip2px(55));
        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.LabelLayout, 0, 0);
        Drawable drawable = a.getDrawable(R.styleable.LabelLayout_layout_icon);
        String title = a.getString(R.styleable.LabelLayout_layout_title);
        String subTitle = a.getString(R.styleable.LabelLayout_layout_subTitle);
        String description = a.getString(R.styleable.LabelLayout_layout_description);
        int indicatorType = a.getInt(R.styleable.LabelLayout_layout_indicatorType,1);
        int lineType = a.getInt(R.styleable.LabelLayout_layout_lineType,0);
        boolean showLineTop = a.getBoolean(R.styleable.LabelLayout_layout_line_top,false);

        LinearLayout contentLayout = new LinearLayout(context);
        contentLayout.setGravity(Gravity.CENTER_VERTICAL);

        LayoutParams lpLine = new LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 1);
        if(showLineTop) {
            dividerViewTop = new View(context);
            int dividerColor = a.getColor(R.styleable.LabelLayout_layout_dividerColor, getResources().getColor(R.color.line_color));
            dividerViewTop.setBackgroundColor(dividerColor);


            if(lineType == 0 ){
                lpLine.leftMargin = 0;
                lpLine.rightMargin = 0;
            }else if(lineType == 1){
                lpLine.leftMargin = ScreenUtil.dip2px(EDGE_DISTANCE);
                lpLine.rightMargin = 0;
            }else if(lineType == 2){
                lpLine.leftMargin = 0;
                lpLine.rightMargin = ScreenUtil.dip2px(EDGE_DISTANCE);
            }else if(lineType == 3){
                lpLine.leftMargin = ScreenUtil.dip2px(EDGE_DISTANCE);
                lpLine.rightMargin = ScreenUtil.dip2px(EDGE_DISTANCE);
            }
            addView(dividerViewTop, lpLine);
        }



        int padding = ScreenUtil.dip2px(10);
        if(drawable != null){
            ivIcon = new ImageView(context);
            ivIcon.setImageDrawable(drawable);
            ivIcon.setPadding(0, padding, 0, padding);
            contentLayout.addView(ivIcon);
        }
        
        llTitle = new LinearLayout(context);
        llTitle.setOrientation(LinearLayout.VERTICAL);
        tvTitle = new TextView(context);
        tvTitle.setText(title);
        tvTitle.setSingleLine();
        tvTitle.setTextSize(TypedValue.COMPLEX_UNIT_SP, 14);
        tvTitle.setTextColor(getResources().getColor(R.color.black90));
        llTitle.addView(tvTitle);
        
        if(subTitle != null){
            tvSubtitle = new TextView(context);
            tvSubtitle.setText(subTitle);
            tvSubtitle.setTextSize(TypedValue.COMPLEX_UNIT_SP,12);
            tvSubtitle.setTextColor(getResources().getColor(R.color.black40));
            LayoutParams lpSubtitle = new LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
            lpSubtitle.topMargin = 8;
            llTitle.addView(tvSubtitle,lpSubtitle);
        }
        
        LayoutParams lp = new LayoutParams(0, ViewGroup.LayoutParams.WRAP_CONTENT,1);
        if(drawable != null) {
            lp.leftMargin = padding;
        }
        contentLayout.addView(llTitle, lp);
        
        String descriptionClass = a.getString(R.styleable.LabelLayout_description_view_class);
        if(!TextUtils.isEmpty(descriptionClass)){
            try {
                isDescriptionTextView = false;
                Class clazz = Class.forName(descriptionClass);
                descriptionView = (View) clazz.getConstructor(Context.class).newInstance(context);
                contentLayout.addView(descriptionView);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }else{
            isDescriptionTextView = true;
            TextView tvDescription = new TextView(context);
            tvDescription.setText(description);
            tvDescription.setTextColor(getResources().getColor(R.color.black40));
            tvDescription.setTextSize(TypedValue.COMPLEX_UNIT_SP, 12);
            tvDescription.setSingleLine(true);
            tvDescription.setEllipsize(TextUtils.TruncateAt.END);
            tvDescription.setMaxEms(10);
            descriptionView = tvDescription;
            contentLayout.addView(descriptionView);
        }
        
        if(indicatorType != 0){
            if(indicatorType == 1){
                indicatorView = new ImageView(context);
                ((ImageView)indicatorView).setImageResource(R.drawable.ic_arrows_right_selector);
            }else if(indicatorType== 2){
                indicatorView = new YtxSwitch(context);
            }
            lp = new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
            lp.leftMargin = padding;
            contentLayout.addView(indicatorView,lp);
        }
        
        lp = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT,1);
        lp.topMargin = ScreenUtil.dip2px(6);
        lp.bottomMargin = ScreenUtil.dip2px(6);

        boolean havePadding = a.getBoolean(R.styleable.LabelLayout_layout_padding,false);
        YtxLog.d("dddddd","havePadding="+havePadding+"ScreenUtil.dip2px(EDGE_DISTANCE)="+ScreenUtil.dip2px(EDGE_DISTANCE));
        if(havePadding){
            contentLayout.setPadding(ScreenUtil.dip2px(EDGE_DISTANCE),0,ScreenUtil.dip2px(EDGE_DISTANCE),0);
        }


        addView(contentLayout,lp);
        
        boolean showLine = a.getBoolean(R.styleable.LabelLayout_layout_line_bottom,true);
        if(showLine) {
            dividerViewBottom = new View(context);
            int dividerColor = a.getColor(R.styleable.LabelLayout_layout_dividerColor, getResources().getColor(R.color.line_color));
            dividerViewBottom.setBackgroundColor(dividerColor);
            if(lineType == 0 ){
                lpLine.leftMargin = 0;
                lpLine.rightMargin = 0;
            }else if(lineType == 1){
                lpLine.leftMargin = ScreenUtil.dip2px(EDGE_DISTANCE);
                lpLine.rightMargin = 0;
            }else if(lineType == 2){
                lpLine.leftMargin = 0;
                lpLine.rightMargin = ScreenUtil.dip2px(EDGE_DISTANCE);
            }else if(lineType == 3){
                lpLine.leftMargin = ScreenUtil.dip2px(EDGE_DISTANCE);
                lpLine.rightMargin = ScreenUtil.dip2px(EDGE_DISTANCE);
            }
            addView(dividerViewBottom, lpLine);
        }
        a.recycle();
    }

    public void setLayoutEnable(boolean isEnbale){
        this.setEnabled(isEnbale);
        tvTitle.setEnabled(isEnbale);
        if(isEnbale) {
            tvTitle.setTextColor(getResources().getColor(R.color.black90));
        }else {
            tvTitle.setTextColor(getResources().getColor(R.color.color_587F7F7F));
        }
        if(tvSubtitle != null){
            tvSubtitle.setEnabled(isEnbale);
            if(isEnbale){
                tvSubtitle.setTextColor(getResources().getColor(R.color.labellayout_subtitle_text_bg));
            }else {
                tvSubtitle.setTextColor(getResources().getColor(R.color.color_587F7F7F));
            }
        }
        if(descriptionView != null && isDescriptionTextView){
            descriptionView.setEnabled(isEnbale);
            if(isEnbale){
                ((TextView)descriptionView).setTextColor(getResources().getColor(R.color.labellayout_subtitle_text_bg));
            }else {
                ((TextView)descriptionView).setTextColor(getResources().getColor(R.color.color_587F7F7F));
            }
        }
        if (indicatorView != null) {
            indicatorView.setEnabled(isEnbale);
        }
    }

    public View getDividerViewBottom() {
        return dividerViewBottom;
    }

    public View getIndicatorView() {
        return indicatorView;
    }

    public ImageView getIconView() {
        return ivIcon;
    }

    public TextView getTitleView() {
        return tvTitle;
    }

    public TextView getSubtitleView() {
        return tvSubtitle;
    }

    public View getDescriptionView() {
        return descriptionView;
    }
}
