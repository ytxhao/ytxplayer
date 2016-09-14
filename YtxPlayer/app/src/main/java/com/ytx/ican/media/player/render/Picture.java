
package com.ytx.ican.media.player.render;

import android.graphics.Bitmap;

/**
 * Photo that holds a GL texture and all its methods must be only accessed from the GL thread.
 */
public class Picture {

    private int texture = -1;
    private int width;
    private int height;

    /**
     * Factory method to ensure every Photo instance holds a valid texture.
     */
    public static Picture create(Bitmap bitmap) {
        return (bitmap != null) ? new Picture(
                RendererUtils.createTexture(bitmap), bitmap.getWidth(), bitmap.getHeight()) : null;
    }

    public static Picture create(int width, int height) {
        return new Picture(RendererUtils.createTexture(), width, height);
    }

    public Picture(int texture, int width, int height) {
        this.texture = texture;
        this.width = width;
        this.height = height;
    }
    public void update(Bitmap bitmap){
        this.texture = RendererUtils.createTexture(texture,bitmap);
        this.width =  bitmap.getWidth();
        this.height = bitmap.getHeight();
    }

    public int texture() {
        return texture;
    }

    public void setTexture(int texture)
    {
    	RendererUtils.clearTexture(this.texture);
    	this.texture = texture;
    }
    public boolean matchDimension(Picture picture) {
        return ((picture.width == width) && (picture.height == height));
    }

    public void changeDimension(int width, int height) {
        this.width = width;
        this.height = height;
        RendererUtils.clearTexture(texture);
        texture = RendererUtils.createTexture();
    }

    public int width() {
        return width;
    }

    public int height() {
        return height;
    }

    public Bitmap save() {
        return RendererUtils.saveTexture(texture, width, height);
    }

    /**
     * Clears the texture; this instance should not be used after its clear() is called.
     */
    public void clear() {
        RendererUtils.clearTexture(texture);
        texture = -1;
    }
    public void updateSize(int width,int height){
        this.width = width;
        this.height = height;
    }
    
    public void swap(Picture picture)
    {
    	int tmp = texture;
    	texture = picture.texture;
        picture.texture = tmp;
    }
}
