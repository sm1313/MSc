import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.net.*;

class Blocks extends JPanel {
    int w, h; // The display width and height of the area
    int gridW, gridH; // width and height of block grid
    int[][] grid; // ints represent strength?
    int remaining;
    DynamicInteger size = new DynamicInteger(); // block size
    int s; // real-time block size (so changes can be delayed til next game)
    DynamicInteger speed = new DynamicInteger();
    Ball ball = new Ball();
    Player player = new Player();

    void create(int w, int h) {
        this.w = w;
        this.h = h;
        setLayout(null);
        setPreferredSize(new Dimension(w, h));
        ball.create(w, h);
        player.create(w);
        resetGrid();
        this.add(player);
        player.setLocation(0, h-(2*player.h));
        this.add(ball);
        ball.setLocation(0, 0);
        setOpaque(false);
    }

    void resetGrid() {
        s = size.val;
        gridW = w/s;
        gridH = h/(2*s);
        grid = new int[gridH][gridW];
        for(int i=0; i<gridH; i++) {
            for(int j=0; j<gridW; j++) {
                grid[i][j] = 1;
            }
        }
        remaining = gridW*gridH;
        ball.edgex = gridW*s;
        player.edge = gridW*s;
        repaint();
    }

    void smash(int x, int y) {
        if(grid[y][x]==0) return;
        grid[y][x] -= 1;
        if(grid[y][x]==0) --remaining;
        repaint();
    }

    void checkMomentum() { // Potential for more complex mechanics here
        ball.swipe(player.xpos - player.last);
    }

    boolean checkBottom() {
        if(ball.y+ball.size.val>=this.getHeight()-player.h) {
            if(ball.x > player.xpos 
                   && ball.x < player.xpos + player.size.val) {
                checkMomentum();
                ball.bounce('y');
                return true;
            }
            else {
                System.out.println("GAAAAME OOOOVER");
                return false;
            }
        }
        return true;
    }

    void checkProximity(int i, int j) {
        int sx = s*j, tx = sx+s;
        int sy = s*i, ty = sy+s;
        if(ball.x>=sx && ball.x<=tx) {
            if(ball.y+ball.size.val>=sy && ball.y-ball.size.val<=ty) {
                smash(j, i);
                ball.bounce('y');
            }
        }
        if(ball.y>=sy && ball.y<=ty) {
            if(ball.x+ball.size.val>=sx && ball.x-ball.size.val<=tx) {
                smash(j, i);
                ball.bounce('x');
            }
        }
    }

    boolean tick() {
        if(remaining==0) {
            System.out.println("YOOOOUUUU WIIIIN");
            return false;
        }
        ball.move();
        for(int i=0; i<gridH; i++) {
            for(int j=0; j<gridW; j++) {
                if(grid[i][j]==1) {
                    checkProximity(i, j);
                }
            }
        }
        return checkBottom();
    }

    void newGame(int w0, int h0) {
        System.out.println("NEEEEW GAAAAME");
        resetGrid();
        ball.reset(player.xpos+1);     
    }

    public void paintComponent(Graphics g0) {
        super.paintComponent(g0);
        Graphics2D g = (Graphics2D) g0;
        g.drawLine(ball.edgex, 0, ball.edgex, h);
        g.drawLine(ball.edgex+1, 0, ball.edgex+1, h);
        g.setPaint(new Color(216,0,0));
        for(int i=0; i<gridH; i++) {
            for(int j=0; j<gridW; j++) {
                drawBlock(g, j, i);
            }
        }
    }

    private void drawBlock(Graphics2D g, int x, int y) {
        if(grid[y][x]>0) {
            g.fillRect(s*x, s*y, s-1, s-1);
        }
    }
}
