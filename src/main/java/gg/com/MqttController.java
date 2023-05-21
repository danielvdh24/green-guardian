package gg.com;

import org.eclipse.paho.client.mqttv3.*;

import java.time.format.DateTimeFormatter;
import java.time.LocalDateTime;

public class MqttController implements Runnable {

    private static final String CLIENT_ID = "PC_APP"; //the app client ID name
    private static final String PUB_TOPIC = "commands"; //topic to pub to
    private static final String SUB_TOPIC = "sensordata"; //topic to subscribe to
    private static final String URL = "tcp://127.0.0.1:1883"; //mqtt broker socket
    private static final int QOS = 0; //sub and pub quality of service
    private static String[] dataBuffer = new String[21];
    MqttClient mqttClient;

    MqttController() throws Exception {

        for (int i = 0; i < 21; i++) dataBuffer[i] = null;
        
        try {
            this.mqttClient = new MqttClient(URL, CLIENT_ID);
            mqttClient.connect();
        } catch (Exception e) {}
    }

    MqttClient getClient() {

        return mqttClient;
    }

    void publish(String message) throws Exception {

        try {
            MqttMessage msg = new MqttMessage(message.getBytes());
            msg.setQos(QOS);
            msg.setRetained(false);
            mqttClient.publish(PUB_TOPIC, msg);
        } catch (Exception e) {}
    }

    void publish(String message, boolean isRetainedInBroker) throws Exception {

        try {
            MqttMessage msg = new MqttMessage(message.getBytes());
            msg.setQos(QOS);
            msg.setRetained(isRetainedInBroker);
            mqttClient.publish(PUB_TOPIC, msg);
        } catch (Exception e) {}
    }

    public void subscribe(App app) throws Exception {

        mqttClient.setCallback(new MqttCallback() {

            public void connectionLost(Throwable cause) {
                app.reconnectBroker();
            }

            public void messageArrived(String topic, MqttMessage message) {
                String text = message.toString();
                dataBuffer[text.charAt(0) - 'a'] = text.substring(1);
                boolean messagecomplete = true;
                for (int i = 0; i < 21 && messagecomplete; i++) if (dataBuffer[i] == null) messagecomplete = false;
                if (messagecomplete) {
                    String result = "";
                    String resultreversed = "";
                    String resultfinal = "";
                    for (int i = 0; i < 21; i++) {
                        result += dataBuffer[i];
                        dataBuffer[i] = null;
                    }
                    result = ',' + result;
                    for (int i = result.length() - 1; i >= 0; i--) resultreversed += result.charAt(i);
                    for (int i = 0; i < resultreversed.length() - 1; i++) {
                        if (resultreversed.charAt(i) == ',') {
                            for (int j = i + 1; j < resultreversed.length(); j++) {
                                if (resultreversed.charAt(j) == ',') {
                                    String temp = resultreversed.substring(i + 1, j);
                                    j = result.length();
                                    for (int k = temp.length() - 1; k >= 0; k--) resultfinal += temp.charAt(k);
                                }
                            }
                            resultfinal += " ";
                        }
                    }
                    DocWriter.write("GraphData.txt", result);
                }
            }

            public void deliveryComplete(IMqttDeliveryToken token) {}
        });

        mqttClient.subscribe(SUB_TOPIC, QOS);
    }

    @Override
    public void run() {

        while (mqttClient.isConnected()) {

            try {
                DateTimeFormatter dtf = DateTimeFormatter.ofPattern("HHmm");
                LocalDateTime now = LocalDateTime.now();
                MqttMessage msg = new MqttMessage(dtf.format(now).getBytes());
                msg.setQos(QOS);
                msg.setRetained(false);
                mqttClient.publish(PUB_TOPIC, msg);
                Thread.sleep(1000); //set to 1000 to make it syncronized with actual time of day
            } catch (Exception e) {}
        }
    }
}