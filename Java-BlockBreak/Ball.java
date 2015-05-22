import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

class Ball extends JComponent {
    DynamicInteger size = new DynamicInteger(); // radius of ball
    double x = -100, y = -100;
    private double dx, dy; // The default for these values is 1.
    int edgex, edgey;

    void create(int edgex, int edgey) {
        this.edgex = edgex;
        this.edgey = edgey;
        setSize(edgex, edgey);
    }

    void swipe(int inc) { // inc could be used for varied speed-change
        if(inc == 0) resetMomentum();
        else if((inc>0 && dx>0) || (inc<0 && dx<0)) {
            dx = 2*dx;
            dy = dy/1.5;
        }
        else dx = -dx;
    }

    void resetMomentum() {
        if(dx!=0) {
            dx /= Math.abs(dx); 
            dy /= Math.abs(dy);
        }
        // If dx is 0, keep it that way
        // If not, reset to 1 or -1 depending on which way it was already going
    }

    void bounce(char c) {
        if(c=='x') dx = -dx;
        if(c=='y') dy = -dy;
    }

    void reset(int x) {
        if(x<=size.val) this.x = size.val+1;
        else this.x=x;
        y=edgey-30;
        dy = 1;
        dx = 1;
        repaint();
    }

    void move() {
        if(x<size.val || edgex-x<size.val) bounce('x');
        if(y<size.val || edgey-y<size.val) bounce('y');
        x+=dx;
        y+=dy;
        repaint();
    }

    public void paintComponent(Graphics g0) {
        super.paintComponent(g0);
        Graphics2D g = (Graphics2D) g0;
        RenderingHints rh = new RenderingHints(
            RenderingHints.KEY_ANTIALIASING,
            RenderingHints.VALUE_ANTIALIAS_ON);
        g.setRenderingHints(rh);
        g.setPaint(new Color(0,255,0));
        g.fillOval((int)x-(size.val), (int)y-(size.val), 
                        size.val*2, size.val*2);
    }
}
