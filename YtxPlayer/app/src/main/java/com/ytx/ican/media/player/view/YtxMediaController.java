package com.ytx.ican.media.player.view;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.MediaController;

/**
 * Created by Administrator on 2016/12/2.
 */

public class YtxMediaController extends MediaController implements IMediaController {
    public YtxMediaController(Context context, AttributeSet attrs) {
        super(context, attrs);
        initView(context);
    }

    public YtxMediaController(Context context, boolean useFastForward) {
        super(context, useFastForward);
        initView(context);
    }

    public YtxMediaController(Context context) {
        super(context);
        initView(context);
    }

    private void initView(Context context) {

    }

    @Override
    public void showOnce(View view) {

    }

    @Override
    public void show() {
        super.show();
    }

    @Override
    public void hide() {
        super.hide();
    }
}
