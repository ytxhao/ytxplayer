package com.ytx.ican.ytxplayer.activity;

import android.os.Bundle;
import android.view.View;

import com.ytx.ican.media.player.pragma.YtxLog;
import com.ytx.ican.ytxplayer.R;
import com.ytx.ican.ytxplayer.constants.ConstKey;
import com.ytx.ican.ytxplayer.utils.PreferenceUtil;
import com.ytx.ican.ytxplayer.view.LabelLayout;
import com.ytx.ican.ytxplayer.view.YtxSwitch;

public class SettingActivity extends BaseActivity implements YtxSwitch.OnSwitchChangedListener, View.OnClickListener {

    private static final String TAG = "SettingActivity";
    private YtxSwitch swDecode;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);
        findView(R.id.llDecode).setOnClickListener(this);
        LabelLayout llDecode = findView(R.id.llDecode);
        boolean isHardDecode = PreferenceUtil.getInstance().getBoolean(ConstKey.IS_HARD_DECODE, false);
        swDecode = (YtxSwitch) llDecode.getIndicatorView();
        swDecode.setOnSwitchChangedListener(this);
        swDecode.setChecked(isHardDecode);
    }


    @Override
    public void onSwitchChanged(YtxSwitch obj, boolean status) {
        PreferenceUtil.getInstance().putBoolean(ConstKey.IS_HARD_DECODE, status);
        if(obj == swDecode){
            YtxLog.d(TAG, "onSwitchChanged obj==swDecode status=" + status);
            PreferenceUtil.getInstance().putBoolean(ConstKey.IS_HARD_DECODE, status);
        }
    }

    @Override
    public void onClick(View v) {

    }
}
