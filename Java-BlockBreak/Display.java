import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

class Display extends JPanel implements MouseMotionListener, MouseListener {
    private Image bg = new ImageIcon(this.getClass().getResource("bg.png")).getImage(); // background image
    private final int wh = 400, ww = 600;
    Blocks blocks;
    private String play = "stop";
    InfoText info = new InfoText(ww/2, wh);

    class InfoText extends JComponent {
        JLabel label = new JLabel("", SwingConstants.CENTER);

        InfoText(int x, int y) {
            this.setLocation(0, y/2);
            this.setSize(x, y);
            this.add(label);
            label.setForeground(Color.white);
            label.setLocation(0, 0);
            label.setSize(x, y/2);
        }

        void set(String text) {
            label.setText(text);
        }
    }

    class Option extends JPanel implements ItemListener {
        JComboBox<String> menu;
        private String name;
        private DynamicInteger ref;
        private int inc;
        private boolean asc = true;

        Option(String name, DynamicInteger ref, int inc, String scend) {
            this.name = name;
            this.ref = ref;
            this.inc = inc;
            if(scend.equals("DESC")) this.asc = false;
            this.add(row());
            this.setOpaque(false);
        }

        JPanel row() {
            GridLayout grid = new GridLayout(1,2);
            JPanel row = new JPanel();
            row.setLayout(grid);
            JLabel label = new JLabel(name + ": ");
            label.setForeground(Color.white);
            row.add(label);
            this.menu = Menu();
            menu.addItemListener(this);
            menu.setSelectedIndex(1); // default is 0 so this is a state change
            row.add(menu);
            row.setOpaque(false);
            return row;
        }

        JComboBox<String> Menu() {
            String[] strings = new String[10];
            for(int i=1; i<=10; i++) {
                if(asc) strings[i-1] = "" + (i*inc);
                else strings[10-i] = "" + (i*inc);
            }
            JComboBox<String> cb = new JComboBox<String>(strings);
            cb.setAlignmentX(JComponent.LEFT_ALIGNMENT);
            cb.setMaximumSize(new Dimension(ww/4, 25));
            return cb;
        }

        public void itemStateChanged(ItemEvent e) {
            if(e.getStateChange() != ItemEvent.SELECTED) return;
            String sref = (String) e.getItem();
            ref.val = Integer.parseInt(sref);
            super.repaint();
        }
    }

    Box options() {
        Box box = Box.createVerticalBox();
        box.setPreferredSize(new Dimension(ww/2, wh));
        box.add(new Option("Ball Speed", blocks.speed, 1, "ASC"));
        box.add(new Option("Player Size", blocks.player.size, 5, "DESC"));
        box.add(new Option("Block Size", blocks.size, ww/100, "DESC"));
        box.add(new Option("Ball Size", blocks.ball.size, 2, "ASC"));
        return box;
    }


    void create() {
        setPreferredSize(new Dimension(ww, wh));
        addMouseMotionListener(this);
        addMouseListener(this);
        Box box = Box.createHorizontalBox();
        blocks = new Blocks();
        box.add(blocks);
        box.add(options());
        blocks.create(ww/2, wh);
        blocks.add(info);
        this.add(box);
    }

    void startGame() {
        blocks.newGame(ww/2, wh);
        for(int i=3; i>0; i--) {
            info.set("" + i);
            try { Thread.sleep(1000); }
            catch(InterruptedException e) { }
        }
        info.set("");
        play = "play";
    }

    void tick() throws InterruptedException {
        if(play.equals("stop")) return;
        if(play.equals("start")) startGame();
        blocks.player.last = blocks.player.xpos; // So ball bounces normally if player static
        Thread.sleep(50/(blocks.speed.val*blocks.speed.val));
        if(blocks.tick()==false) {
            info.set("GAME OVER");
            play = "stop";
        }
    }

    public void paintComponent(Graphics g0) {
        super.paintComponent(g0);
        Graphics2D g = (Graphics2D) g0;
        g.drawImage(bg, 0, 0, null);
    }

    public void mouseClicked(MouseEvent e) {
        if(play.equals("play")) return;
        play = "start";
    }
    public void mousePressed(MouseEvent e) { }
    public void mouseReleased(MouseEvent e) { }
    public void mouseEntered(MouseEvent e) { }
    public void mouseExited(MouseEvent e) { }

    public void mouseMoved(MouseEvent e) { 
        blocks.player.move(e.getX());
    }
    public void mouseDragged(MouseEvent e) { }
}
