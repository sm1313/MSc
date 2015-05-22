import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

class Game implements Runnable {
    private Display display;

    void play() {
        System.out.println("WELCOME.");
        try { Thread.sleep(1000); } catch(Exception e) { } 
        while(true) {
            try { 
                display.tick();
                Thread.sleep(1);
            }
            catch(Exception e) { }
        }
    }

    public void run() {
        JFrame win = new JFrame("BlockBreak");
        win.setDefaultCloseOperation(win.EXIT_ON_CLOSE);
        display = new Display();
        win.add(display);
        display.create();
        win.pack();
        win.setLocationByPlatform(true);
        win.setVisible(true);
    }

    public static void main(String[] args) {
        Game game = new Game();
        SwingUtilities.invokeLater(game);
        game.play();
    }
}
