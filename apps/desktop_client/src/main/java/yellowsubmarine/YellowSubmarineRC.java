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
     * IP adress of the boat esp32
     */
    private String ip;

    /**
     * Port the of the boat esp32 service
     */
    private int port;

    /**
     * Enum for the available pressure tank operations.
     */
    private enum PressureTankOperation {IN, OUT, NONE}

    /**
     * speed, side rudder, pitch elevator
     */
    private int[] params = {0, 0, 0};

    /**
     * Current pressure tank operation.
     */
    private PressureTankOperation pressureTankOperation;

    public YellowSubmarineRC() {
        params[0] = 0;
        params[1] = 0;
        params[2] = 0;
        pressureTankOperation = PressureTankOperation.NONE;
        ip = "192.168.178.61";
        port = 1234;

        JPanel mainPanel = new JPanel();
        getContentPane().add(mainPanel);
        mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));

        // Custom controls
        mainPanel.add(makeUI(0, "Speed", 0, 100));
        mainPanel.add(makeUI(1, "Side rudder", -45, 45));
        mainPanel.add(makeUI(2, "Pitch elevator", -45, 45));
        mainPanel.add(makePressureTankOperationUI());
        mainPanel.add(makeSendCommandUI());

        setTitle("Yellow Submarine");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(400, 200);
        setVisible(true);
    }

    /**
     * Create the user interface for the UDP connection and command sending.
     */
    private Component makeSendCommandUI() {
        JPanel panel = new JPanel();
        panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));

        JTextField textFieldIp = new JTextField();
        textFieldIp.setText(ip);
        textFieldIp.addActionListener(e -> {
            ip = textFieldIp.getText();
        });
        panel.add(textFieldIp);

        JTextField textFieldPort = new JTextField();
        textFieldPort.setText("" + port);
        textFieldPort.addActionListener(e -> {
            port = Integer.valueOf(textFieldPort.getText());
        });
        panel.add(textFieldPort);

        JButton button = new JButton("Send");
        button.addActionListener(e -> {
            String command = "";
            System.out.println("Send (" + ip + ", " + port + "): " + makeCommandString());
            sendCommand();
        });
        panel.add(button);
        return panel;
    }

    /**
     * Send command via UDP
     */
    private void sendCommand() {
        byte[] buffer = {(byte) params[0], (byte) params[1], (byte) params[2], (byte) pressureTankOperation.ordinal()};
        byte[] IP = {-64, -88, 1, 106};
        try {
            InetAddress address = InetAddress.getByName(ip);
            DatagramPacket packet = new DatagramPacket(
                    buffer, buffer.length, address, port
            );
            DatagramSocket datagramSocket = new DatagramSocket();
            datagramSocket.send(packet);
        } catch (UnknownHostException e) {
            System.out.println("Error sending command: " + e.getMessage());
        } catch (SocketException e) {
            System.out.println("Error sending command: " + e.getMessage());
        } catch (IOException e) {
            System.out.println("Error sending command: " + e.getMessage());
        }
    }

    /**
     * Make a command string from the current params. Used to be sent over UDP.
     */
    private String makeCommandString() {
        return params[0] + " " + params[1] + " " + params[2] + " " + pressureTankOperation.toString();
    }

    /**
     * Generate a user interface for a numerical parameter with the goven index.
     * @param index Index of the parameter.
     * @param caption Parameter description.
     * @param min Min value for the param.
     * @param max Max value for the param.
     * @return
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
        });
        return panel;
    }

    /**
     * Make a user interface component for the pressure operation.
     */
    private Component makePressureTankOperationUI() {
        JLabel label = new JLabel("Pressure tank operation: ");
        JComboBox<String> combo = new JComboBox<>();
        for (PressureTankOperation op : PressureTankOperation.values()) {
            combo.addItem(op.toString());
        }
        combo.setSelectedItem(pressureTankOperation.toString());
        JPanel panel = new JPanel();
        panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
        panel.add(label);
        panel.add(combo);
        combo.addActionListener(e -> {
            pressureTankOperation = PressureTankOperation.valueOf(combo.getSelectedItem().toString());
        });
        return panel;
    }

    public static void main(String[] args) {
        new YellowSubmarineRC();
    }
}
