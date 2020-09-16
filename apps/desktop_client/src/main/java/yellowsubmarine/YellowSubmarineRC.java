package yellowsubmarine;

import javax.swing.*;
import java.awt.*;
import java.io.IOException;
import java.net.*;

/**
 * Desktop remote control application, sends command via UDP to ESP32.
 */
public class YellowSubmarineRC extends JFrame {

    /**
     * Command update interval in ms.
     */
    private static final int COMMAND_UPDATE_INTERVAL = 100;

    /**
     * IP address of the boat esp32
     */
    private String ip;

    /**
     * Port the of the boat esp32 service
     */
    private int port;

    /**
     * speed, side rudder, pitch elevator, tank motor
     */
    private final int[] params = {0, 0, 0, 0};

    /**
     * This flag indicates that something has changed.
     */
    private boolean sendCommandRequired;

    public YellowSubmarineRC() {
        params[0] = 0;
        params[1] = 0;
        params[2] = 0;
        params[3] = 0;
        ip = "192.168.178.61";
        port = 1234;
        sendCommandRequired = false;

        JPanel mainPanel = new JPanel();
        getContentPane().add(mainPanel);
        mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));

        // Custom controls
        mainPanel.add(makeUI(0, "Speed", -100, 100));
        mainPanel.add(makeUI(1, "Side rudder", -90, 90));
        mainPanel.add(makeUI(2, "Pitch elevator", -90, 90));
        mainPanel.add(makeUI(3, "Pressure tank", -100, 100));
        mainPanel.add(makeSendCommandUI());

        setTitle("Yellow Submarine");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(400, 200);
        setVisible(true);

        // Send update (if required) via UDP to ESP32
        Timer timer = new Timer(COMMAND_UPDATE_INTERVAL, e -> sendCommand());
        timer.start();
    }

    /**
     * Create the user interface for the UDP connection and command sending.
     */
    private Component makeSendCommandUI() {
        JPanel panel = new JPanel();
        panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));

        JTextField textFieldIp = new JTextField();
        textFieldIp.setText(ip);
        textFieldIp.addActionListener(e -> ip = textFieldIp.getText());
        panel.add(textFieldIp);

        JTextField textFieldPort = new JTextField();
        textFieldPort.setText("" + port);
        textFieldPort.addActionListener(e -> port = Integer.parseInt(textFieldPort.getText()));
        panel.add(textFieldPort);

        JButton button = new JButton("Send");
        button.addActionListener(e -> sendCommand());
        panel.add(button);
        return panel;
    }

    /**
     * Send command via UDP
     */
    private void sendCommand() {
        if (!sendCommandRequired) {
            return;
        }
        sendCommandRequired = false;

        byte[] buffer = {(byte) params[0], (byte) params[1], (byte) params[2], (byte) params[3]};
        try {
            InetAddress address = InetAddress.getByName(ip);
            DatagramPacket packet = new DatagramPacket(
                    buffer, buffer.length, address, port
            );
            DatagramSocket datagramSocket = new DatagramSocket();
            datagramSocket.send(packet);
            Thread.sleep(20);
        } catch (IOException | InterruptedException e) {
            System.out.println("Error sending command: " + e.getMessage());
        }
    }

    /**
     * Generate a user interface for a numerical parameter with the given index.
     *
     * @param index   Index of the parameter.
     * @param caption Parameter description.
     * @param min     Min value for the param.
     * @param max     Max value for the param.
     * @return UI as a component.
     */
    private Component makeUI(int index, String caption, int min, int max) {
        JLabel label = new JLabel(caption + " (" + params[index] + "): ");
        JSlider slider = new JSlider();
        JPanel panel = new JPanel();
        slider.setValue(params[index]);
        slider.setMinimum(min);
        slider.setMaximum(max);
        panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
        panel.add(label);
        panel.add(slider);
        slider.addChangeListener(e -> {
            params[index] = slider.getValue();
            label.setText(caption + " (" + params[index] + "): ");
            sendCommandRequired = true;
        });
        return panel;
    }

    public static void main(String[] args) {
        new YellowSubmarineRC();
    }
}
