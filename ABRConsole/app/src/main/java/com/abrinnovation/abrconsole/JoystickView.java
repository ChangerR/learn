package com.abrinnovation.abrconsole;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.util.jar.Attributes;

/**
 * Created by Changer on 2015/12/14.
 */
public class JoystickView extends SurfaceView implements SurfaceHolder.Callback {

    private int width, height;
    private int leid = -1,lex,ley,lexl,leyl;
    private int reid = -1,rex,rey,rexl,reyl;
    private Paint paint = null;
    private Context _context = null;

    public JoystickView(Context context,AttributeSet attrs) {
        super(context, attrs);
        SurfaceHolder holder = getHolder();
        holder.addCallback(this);
        setFocusable(true); // 确保我们的View能获得输入焦点
        setFocusableInTouchMode(true); // 确保能接收到触屏事件
        init();

        _context = context;
    }

    private void init() {
        paint = new Paint();
        paint.setAntiAlias(true);
        setZOrderOnTop(true);
    }

    /**
     * 处理触屏事件
     */
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        // 锁定Canvas,开始进行相应的界面处理
        Canvas c = getHolder().lockCanvas();
        int pointerCount = event.getPointerCount();

        if (c != null) {
            //c.drawColor(Color.TRANSPARENT);
            c.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
            if (event.getAction() == MotionEvent.ACTION_UP || event.getActionMasked() == MotionEvent.ACTION_POINTER_UP) {

                Log.i("ABRConsole","Get " + pointerCount + " MotionEvent");
                for(int i = 0; i < pointerCount;i++) {
                    int id = event.getPointerId(i);

                    Log.i("ABRConsole","This event id=" + id);
                    if(id == leid) {
                        leid = -1;
                    }

                    if(id == reid) {
                        reid = -1;
                    }
                }
            } else if(event.getAction() == MotionEvent.ACTION_DOWN || event.getActionMasked() == MotionEvent.ACTION_POINTER_DOWN) {

                for(int i = 0; i < pointerCount; i++) {
                    int id = event.getPointerId(i);
                    int x = (int) event.getX(i);
                    int y = (int) event.getY(i);

                    //Log.i("ABRConsole","This event x=" + x +" width/2=" + width/2 + " id=" + id);
                    if( x < this.width / 2 && leid == -1 ) {
                        leid = id;
                        lexl = lex = x;
                        leyl = ley = y;
                    }

                    if( x >= this.width / 2 && reid == -1) {
                        //Log.i("ABRConsole","set reid = " + id);
                        reid = id;
                        rexl = rex = x;
                        reyl = rey = y;

                    }
                }
            } if(event.getAction() == MotionEvent.ACTION_MOVE) {
                //Log.i("ABRConsole","Get " + pointerCount + " MotionEvent MOVE led=" + leid + " reid=" + reid);
                if(leid != -1 ||reid != -1) {
                    for(int i = 0; i < pointerCount;i++) {
                        int id = event.getPointerId(i);
                        int x = (int) event.getX(i);
                        int y = (int) event.getY(i);

                        //Log.i("ABRConsole","This event id=" + id);
                        if(id == leid) {
                            lexl = x;
                            leyl = y;
                        }

                        if(id == reid) {
                            rexl = x;
                            reyl = y;
                        }
                    }
                }
            }

            drawJoystick(c);

            // 画完后，unlock
            getHolder().unlockCanvasAndPost(c);
        }
        return true;
    }

    private void drawJoystick(Canvas c) {
        if(reid != -1)
            drawJoystickR(rex,rey,rexl,reyl,c);
        if(leid != -1)
            drawJoystickL(lex,ley,lexl,leyl,c);
    }

    private void drawJoystickL(int centerx,int centery,int x,int y, Canvas c) {

        int ringWidth = dip2px(_context,6);
        int innerRadius = dip2px(_context,60);
        float midRadius = innerRadius + ringWidth / 2;
        float distance = (float)Math.sqrt(Math.pow(x - centerx, 2) + Math.pow(y - centery,2));
        float rx = x;
        float ry = y;
        paint.setARGB(155,255,0,0);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(ringWidth);
        c.drawCircle(centerx,centery,innerRadius,paint);

        if(distance > midRadius) {
            rx = ((rx - centerx) / distance) * midRadius + centerx;
            ry = ((ry - centery) / distance) * midRadius + centery;
        }

        paint.setStyle(Paint.Style.FILL);
        c.drawCircle(rx,ry,dip2px(_context,20),paint);
    }

    private void drawJoystickR(int centerx,int centery,int x,int y, Canvas c) {
        int lineWidth = dip2px(_context,6);
        int lineRange = dip2px(_context,60);
        float radiusY = y;
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(lineWidth);
        paint.setARGB(155, 255, 0, 0);
        c.drawLine(centerx, centery - lineRange, centerx, centery + lineRange, paint);

        if(radiusY > centery + lineRange)
            radiusY = centery + lineRange;

        if(radiusY < centery - lineRange)
            radiusY = centery - lineRange;

        paint.setStyle(Paint.Style.FILL);
        c.drawCircle(centerx, radiusY,dip2px(_context,20),paint);
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

        this.width = width;
        this.height = height;
        getHolder().setFormat(PixelFormat.TRANSLUCENT);
        Canvas c = getHolder().lockCanvas();
        if (c != null) {
            //c.drawColor(0x00000000);
            c.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
            getHolder().unlockCanvasAndPost(c);
        }
    }

    public void surfaceCreated(SurfaceHolder holder) {
        //super.setZOrderMediaOverlay(true);
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
    }

    public static int dip2px(Context context, float dpValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }
}
