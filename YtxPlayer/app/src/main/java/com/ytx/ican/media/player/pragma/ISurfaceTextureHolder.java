
package com.ytx.ican.media.player.pragma;

import android.graphics.SurfaceTexture;

public interface ISurfaceTextureHolder {
    void setSurfaceTexture(SurfaceTexture surfaceTexture);

    SurfaceTexture getSurfaceTexture();

    void setSurfaceTextureHost(ISurfaceTextureHost surfaceTextureHost);
}
