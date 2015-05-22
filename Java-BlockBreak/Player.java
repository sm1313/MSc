import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

class Player extends JComponent {
    int xpos, ypos;
    int last; // The position of the Player at the last tick.
    int h = 5;
    int edge;
    DynamicInteger size = new DynamicInteger();

    void create(int w) { // w is the horizontal limit of its area.
        setSize(w, h);
        edge = w;
        xpos = 0;
        ypos = 0;
        last = 0;
    }

    void move(int x) {
        int w = size.val;
        if(x>(edge-(w/2))) xpos = edge - size.val;
        else if(x<(w/2)) xpos = 0;
        else xpos = x-(w/2);
        repaint();
    }

    public void paintComponent(Graphics g0) {
        super.paintComponent(g0);
        Graphics2D g = (Graphics2D) g0;
        g.setPaint(new Color(128,128,255));
        g.fillRect(xpos, ypos, (size.val), h);
    }
}
