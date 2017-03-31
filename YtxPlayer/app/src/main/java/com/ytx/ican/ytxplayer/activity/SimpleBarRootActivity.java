package com.ytx.ican.ytxplayer.activity;

import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.util.TypedValue;
import android.view.View;
import android.view.ViewStub;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.ytx.ican.ytxplayer.R;
import com.ytx.ican.ytxplayer.utils.ScreenUtil;


public class SimpleBarRootActivity extends BaseActivity implements View.OnClickListener{

    private static String TAG = "SimpleBarRootActivity";

    protected Toolbar titleBar;
    private TextView tvTitle;
    private LinearLayout llMenu;
    private ImageView ivNavigation;
    private View baseLine;
    
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		super.setContentView(R.layout.activity_simple_bar_root);
		titleBar = findView(R.id.barTitle);
        ivNavigation = (ImageView) titleBar.findViewById(R.id.ivNavigation);
        ivNavigation.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onNavigationIconClick(v);
            }
        });
        tvTitle = (TextView) titleBar.findViewById(R.id.tvTitle);
        llMenu = (LinearLayout) titleBar.findViewById(R.id.llMenu);
        baseLine = findView(R.id.baseLine);
        setSupportActionBar(titleBar);
    }

    /**
     * 设置titlebar下面直线的颜色
     * @param id
     */

    public void setBaseLineColor(int id) {
        baseLine.setBackgroundColor(id);
    }

    /**
     * 设置是否显示titlebar下面的直线
     * @param isHide
     */
    
    public void hideBaseLine(boolean isHide){
        baseLine.setVisibility(isHide ? View.GONE : View.VISIBLE);
    }
    
    public void setBackground(int id){
        findView(R.id.rlBase).setBackgroundResource(id);
    }

    public void setBackgroundColor(int color){
        findView(R.id.rlBase).setBackgroundColor(color);
    }

    public void setTitleBarBackground(int id){
        titleBar.setBackgroundResource(id);
    }
    @Override
    public void setContentView(int layoutResID) {
        ViewStub vs =  findView(R.id.layout_root_content);
        vs.setLayoutResource(layoutResID);
        vs.inflate();
    }

    @Override
	public void setTitle(CharSequence title) {
        tvTitle.setText(title);
	}

	@Override
	public void setTitle(int titleId) {
		tvTitle.setText(titleId);
    }

    /**
     * 设置title字体的颜色
     * @param color
     */

    public void setTitleTextColor(int color) {
        tvTitle.setTextColor(color);
    }

    /**
     * 隐藏titleBar
     * @param isHide
     */
    public void hideTitleBar(boolean isHide){
        titleBar.setVisibility(isHide ? View.GONE : View.VISIBLE);
    }

    /**
     * 设置返回按钮（title上左边的按钮）
     * @param resID
     */
    public void setNavigationIcon(int resID){
        ivNavigation.setImageResource(resID);
    }

    /**
     * 隐藏或显示返回按钮
     * @param isHide
     */
    public void hideNavigationIcon(boolean isHide){
        ivNavigation.setVisibility(isHide ? View.GONE : View.VISIBLE);
    }

    /**
     * 返回按钮（title上左边的按钮）点击事件的处理
     * @param v
     */
    public void onNavigationIconClick(View v) {
        finish();
    }

    /**
     * 添加图标菜单项（title上右边的图标按钮、可以添加多个）
     * @param iconId 菜单图标
     */
    public void addMenu(int id,int iconId){
        ImageView iv = new ImageView(this);
        iv.setId(id);
        iv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onMenuItemClick(v);
            }
        });
        iv.setImageResource(iconId);
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        lp.leftMargin = ScreenUtil.dip2px(10);
        iv.setPadding(25,0,0,0);
        llMenu.addView(iv, lp);
    }
    /**
     * 添加文字菜单项（title上右边的文字按钮、可以添加多个）
     * @param textId 菜单文字
     */
    public void addTextMenu(int id,int textId){
        TextView tv = new TextView(this);
        tv.setId(id);
        tv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onMenuItemClick(v);
            }
        });
        tv.setText(textId);
        tv.setTextColor(getResources().getColor(R.color.color_B2242424));
        tv.setTextSize(TypedValue.COMPLEX_UNIT_SP, 13);
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        lp.leftMargin = ScreenUtil.dip2px(10);
        llMenu.addView(tv,lp);
    }

    /**
     * 获取title右边的菜单项（ImageView 或 TextView）
     * @param id
     * @return
     */
    public View getMenu(int id){
        return llMenu.findViewById(id);
    }

    /**
     * 删除指定id 的菜单项
     * @param id
     */
    public void removeMenu(int id){
        llMenu.removeView(llMenu.findViewById(id));
    }

    /**
     * 删除所有菜单项
     */
    public void removeAllMenu(){
        llMenu.removeAllViews();
    }

    /**
     * title 上右边的菜单项点击事件， 子类需覆盖重写对点击事件做处理
     * @param item
     */
    public void onMenuItemClick(View item){
        
    }

    @Override
    public void onClick(View v){
        
    }

    public void setBaseLineTitleBarColor(int color){
        titleBar.setBackgroundColor(color);
        setBaseLineColor(color);
    }
}
